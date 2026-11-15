#include "Commands/Conversion/ConvertClusteringToCadCommand.h"

#include "CADacomb/CAD/Conversion/ClusterToCadConverter.h"
#include "CADacomb/CAD/Interface/Shape.h"
#include "CADacomb/CAD/Sm/KernelSm.h"
#include "CADacomb/Triangle/Triangulation.h"
#include "CADacomb/Util/ProgressBar.h"
#include "Database/Asset/CadAsset.h"
#include "Database/Asset/TriangulationAsset.h"
#include "Database/Component/FaceValuesComponent.h"
#include "Database/Database.h"

#include <map>

ConvertClusteringToCadCommand::ConvertClusteringToCadCommand() {}

ConvertClusteringToCadCommand::~ConvertClusteringToCadCommand() {}

std::string ConvertClusteringToCadCommand::getType() const
{
    return "ConvertClusteringToCad";
}

int ConvertClusteringToCadCommand::getVersion() const
{
    return 1;
}

std::string ConvertClusteringToCadCommand::getDescription() const
{
    return "Command: ConvertClusteringToCad\n"
           "Version: 1\n"
           "Purpose: Converts a TriangulationAsset's FaceValuesComponent clustering into a CadAsset.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"ConvertClusteringToCad\",\n"
           "  \"version\": 1,\n"
           "  \"assetGuid\": <string>,\n"
           "  \"componentGuid\": <string> (optional, defaults to first FaceValuesComponent on the asset),\n"
           "  \"planarityTolerance\": <number> (optional, default 1e-3),\n"
           "  \"boundarySimplifyTolerance\": <number> (optional, default 1e-4),\n"
           "  \"stitchTolerance\": <number> (optional, default 1e-3),\n"
           "  \"tryMakeSolid\": <bool> (optional, default true)\n"
           "}\n\n"
           "Output (JSON):\n"
           "{\n"
           "  \"type\": \"ConvertClusteringToCad\",\n"
           "  \"version\": 1,\n"
           "  \"assetGuid\": <string>,\n"
           "  \"componentGuid\": <string>,\n"
           "  \"createdAssetGuid\": <string>\n"
           "}";
}

std::vector<CADacomb::GUID> ConvertClusteringToCadCommand::getRequiredAssets() const
{
    return { assetGuid };
}

void ConvertClusteringToCadCommand::setInput(const Polyglot::Node& node)
{
    assetGuid                 = CADacomb::GUID(node["assetGuid"].asString());
    componentGuid             = CADacomb::GUID();
    planarityTolerance        = 1e-3;
    boundarySimplifyTolerance = 1e-4;
    stitchTolerance           = 1e-3;
    tryMakeSolid              = true;

    if (node.contains("componentGuid") && !node["componentGuid"].isNull() && !node["componentGuid"].asString().empty())
    {
        componentGuid = CADacomb::GUID(node["componentGuid"].asString());
    }
    if (node.contains("planarityTolerance") && !node["planarityTolerance"].isNull())
    {
        planarityTolerance = node["planarityTolerance"].asDouble();
    }
    if (node.contains("boundarySimplifyTolerance") && !node["boundarySimplifyTolerance"].isNull())
    {
        boundarySimplifyTolerance = node["boundarySimplifyTolerance"].asDouble();
    }
    if (node.contains("stitchTolerance") && !node["stitchTolerance"].isNull())
    {
        stitchTolerance = node["stitchTolerance"].asDouble();
    }
    if (node.contains("tryMakeSolid") && !node["tryMakeSolid"].isNull())
    {
        tryMakeSolid = node["tryMakeSolid"].asBool();
    }
}

nlohmann::json ConvertClusteringToCadCommand::execute(Database& database)
{
    CADacomb::ProgressBar bar(3, "Converting clustering to CAD");

    Project&               project = database.getProject();
    std::shared_ptr<Asset> asset   = project.getAsset(assetGuid);
    if (!asset)
    {
        return createErrorResponse("AssetNotFound", "Asset not found: " + assetGuid.toString());
    }

    std::shared_ptr<TriangulationAsset> triAsset = std::dynamic_pointer_cast<TriangulationAsset>(asset);
    if (!triAsset)
    {
        return createErrorResponse("IncompatibleAsset", "Asset is not a TriangulationAsset: " + assetGuid.toString());
    }

    std::shared_ptr<FaceValuesComponent> faceValues;
    for (const auto& attachment : triAsset->getAttachments())
    {
        auto candidate = std::dynamic_pointer_cast<FaceValuesComponent>(attachment);
        if (!candidate)
        {
            continue;
        }
        if (componentGuid.toString().empty())
        {
            faceValues    = candidate;
            componentGuid = candidate->getGuid();
            break;
        }
        if (candidate->getGuid() == componentGuid)
        {
            faceValues = candidate;
            break;
        }
    }

    if (!faceValues)
    {
        return createErrorResponse("ComponentNotFound", "FaceValuesComponent not found on asset: " + assetGuid.toString());
    }

    const CADacomb::Triangulation& tri              = triAsset->getTriangulation();
    const std::vector<uint32_t>&   faceValueIndices = faceValues->getFaceValueIndices();
    size_t                         numTriangles     = tri.indices.size() / 3;
    if (faceValueIndices.size() != numTriangles)
    {
        return createErrorResponse("InvalidClustering", "FaceValuesComponent face count does not match triangulation");
    }

    std::map<uint32_t, std::vector<size_t>> clustersById;
    for (size_t triIdx = 0; triIdx < numTriangles; triIdx++)
    {
        clustersById[faceValueIndices[triIdx]].push_back(triIdx);
    }

    std::vector<std::vector<size_t>> clusters;
    clusters.reserve(clustersById.size());
    for (const auto& [clusterId, triIndices] : clustersById)
    {
        clusters.push_back(triIndices);
    }
    bar.addProgress(1);

    if (bar.isCanceled())
    {
        return createErrorResponse("Canceled", "Converting clustering to CAD was canceled");
    }

    CADacomb::CAD::KernelSm               kernel;
    CADacomb::CAD::ClusterToCadConverter  converter;
    std::unique_ptr<CADacomb::CAD::Shape> shape = converter.convert(kernel, tri, clusters, planarityTolerance, boundarySimplifyTolerance, stitchTolerance, tryMakeSolid);
    bar.addProgress(1);

    createdAsset = std::make_shared<CadAsset>();
    createdAsset->setName(triAsset->getName() + "_CAD");
    createdAsset->setCadShape(std::shared_ptr<CADacomb::CAD::Shape>(std::move(shape)));
    project.addAsset(createdAsset);
    bar.addProgress(1);

    return createResponse({
      {        "assetGuid",              assetGuid.toString() },
      {    "componentGuid",          componentGuid.toString() },
      { "createdAssetGuid", createdAsset->getGuid().toString() }
    });
}

bool ConvertClusteringToCadCommand::isUndoable() const
{
    return true;
}

void ConvertClusteringToCadCommand::undo(Database& database)
{
    if (createdAsset)
    {
        database.getProject().removeAsset(createdAsset->getGuid());
    }
}

void ConvertClusteringToCadCommand::redo(Database& database)
{
    if (createdAsset)
    {
        database.getProject().addAsset(createdAsset);
    }
}

size_t ConvertClusteringToCadCommand::getUndoSize() const
{
    return createdAsset ? createdAsset->getSizeInBytes() : 0;
}

#ifdef ISTESTPROJECT
#include "CADacomb/Triangle/BaseShapeGenerator.h"
#include "CADacomb/Triangle/Clustering.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("ConvertClusteringToCadCommand/BoxToCadAsset")
{
    Database db;

    auto tri      = std::make_unique<CADacomb::Triangulation>(CADacomb::BaseShapeGenerator::cube());
    auto triAsset = std::make_shared<TriangulationAsset>();
    triAsset->setName("Box");
    triAsset->setTriangulation(std::move(tri));

    std::vector<std::vector<size_t>> clusters = CADacomb::Clustering::clusterByNormal(triAsset->getTriangulation(), 0.99);
    REQUIRE(clusters.size() == 6);

    std::vector<uint32_t> faceValueIndices(triAsset->getTriangulation().indices.size() / 3, 0);
    for (size_t c = 0; c < clusters.size(); c++)
    {
        for (size_t triIdx : clusters[c])
        {
            faceValueIndices[triIdx] = static_cast<uint32_t>(c);
        }
    }

    auto comp = std::make_shared<FaceValuesComponent>();
    comp->setFaceValueIndices(faceValueIndices);
    triAsset->addComponent(comp);
    db.getProject().addAsset(triAsset);

    ConvertClusteringToCadCommand cmd;
    cmd.setInput(Polyglot::Node(nlohmann::json{
      {          "type",            "ConvertClusteringToCad" },
      {       "version",                                   1 },
      {     "assetGuid",    triAsset->getGuid().toString() },
      { "componentGuid",        comp->getGuid().toString() }
    }));

    nlohmann::json res = cmd.execute(db);
    REQUIRE(res.at("type") == "ConvertClusteringToCad");

    CADacomb::GUID createdGuid(res.at("createdAssetGuid").get<std::string>());
    auto           cadAsset = std::dynamic_pointer_cast<CadAsset>(db.getProject().getAsset(createdGuid));
    REQUIRE(cadAsset != nullptr);
    REQUIRE(cadAsset->getTriangulation().indices.size() >= 36);

    CADacomb::CAD::KernelSm kernel;
    std::string             tempStep = "temp_roundtrip_cube.step";
    kernel.saveStepFile(cadAsset->getCadShape(), tempStep, "cube_roundtrip");
    std::vector<std::unique_ptr<CADacomb::CAD::Shape>> reloaded = kernel.loadStepFile(tempStep);
    std::remove(tempStep.c_str());
    REQUIRE(!reloaded.empty());
    REQUIRE(reloaded[0] != nullptr);

    cmd.undo(db);
    REQUIRE(db.getProject().getAsset(createdGuid) == nullptr);

    cmd.redo(db);
    REQUIRE(db.getProject().getAsset(createdGuid) != nullptr);
}

#include "CADacomb/CAD/Interface/Solid.h"
#include "CADacomb/CAD/Interface/TessellationOptions.h"

TEST_CASE("ConvertClusteringToCadCommand/CylinderToCadAsset")
{
    Database                db;
    CADacomb::CAD::KernelSm kernel;
    auto                    cylSolid = kernel.createCylinder(glm::dvec3(0.0, 0.0, 0.0), 5.0, 10.0);

    CADacomb::CAD::TessellationOptions tessOpts;
    auto                               tri      = kernel.tessellate(*cylSolid, tessOpts);
    auto                               triAsset = std::make_shared<TriangulationAsset>();
    triAsset->setName("Cylinder");
    triAsset->setTriangulation(std::move(tri));

    std::vector<std::vector<size_t>> clusters = CADacomb::Clustering::clusterByNormal(triAsset->getTriangulation(), 0.70);
    REQUIRE(clusters.size() == 3);

    std::vector<uint32_t> faceValueIndices(triAsset->getTriangulation().indices.size() / 3, 0);
    for (size_t c = 0; c < clusters.size(); c++)
    {
        for (size_t triIdx : clusters[c])
        {
            faceValueIndices[triIdx] = static_cast<uint32_t>(c);
        }
    }

    auto comp = std::make_shared<FaceValuesComponent>();
    comp->setFaceValueIndices(faceValueIndices);
    triAsset->addComponent(comp);
    db.getProject().addAsset(triAsset);

    ConvertClusteringToCadCommand cmd;
    cmd.setInput(Polyglot::Node(nlohmann::json{
      {          "type",         "ConvertClusteringToCad" },
      {       "version",                                1 },
      {     "assetGuid", triAsset->getGuid().toString() },
      { "componentGuid",     comp->getGuid().toString() }
    }));

    nlohmann::json res = cmd.execute(db);
    REQUIRE(res.at("type") == "ConvertClusteringToCad");
}
#endif
