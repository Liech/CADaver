#include "Commands/Geometry/SplitClustersToAssetsCommand.h"

#include "CADacomb/Triangle/Triangulation.h"
#include "CADacomb/Util/ProgressBar.h"
#include "Database/Asset/TriangulationAsset.h"
#include "Database/Component/FaceValuesComponent.h"
#include "Database/Database.h"

#include <map>
#include <unordered_map>

SplitClustersToAssetsCommand::SplitClustersToAssetsCommand() {}

SplitClustersToAssetsCommand::~SplitClustersToAssetsCommand() {}

std::string SplitClustersToAssetsCommand::getType() const
{
    return "SplitClustersToAssets";
}

int SplitClustersToAssetsCommand::getVersion() const
{
    return 1;
}

std::string SplitClustersToAssetsCommand::getDescription() const
{
    return "Command: SplitClustersToAssets\n"
           "Version: 1\n"
           "Purpose: Splits a TriangulationAsset's FaceValuesComponent clustering into separate TriangulationAssets.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"SplitClustersToAssets\",\n"
           "  \"version\": 1,\n"
           "  \"assetGuid\": <string>,\n"
           "  \"componentGuid\": <string> (optional, defaults to first FaceValuesComponent on the asset)\n"
           "}\n\n"
           "Output (JSON):\n"
           "{\n"
           "  \"type\": \"SplitClustersToAssets\",\n"
           "  \"version\": 1,\n"
           "  \"assetGuid\": <string>,\n"
           "  \"componentGuid\": <string>,\n"
           "  \"createdAssetGuids\": [ <string> ],\n"
           "  \"createdCount\": <integer>\n"
           "}";
}

std::vector<CADacomb::GUID> SplitClustersToAssetsCommand::getRequiredAssets() const
{
    return { assetGuid };
}

void SplitClustersToAssetsCommand::setInput(const Polyglot::Node& node)
{
    assetGuid     = CADacomb::GUID(node["assetGuid"].asString());
    componentGuid = CADacomb::GUID();

    if (node.contains("componentGuid") && !node["componentGuid"].isNull() && !node["componentGuid"].asString().empty())
    {
        componentGuid = CADacomb::GUID(node["componentGuid"].asString());
    }
}

nlohmann::json SplitClustersToAssetsCommand::execute(Database& database)
{
    CADacomb::ProgressBar bar(2, "Splitting clusters into assets");

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
    {
        CADacomb::ProgressBar groupBar(numTriangles, "Grouping cluster triangles");
        for (size_t triIdx = 0; triIdx < numTriangles; triIdx++)
        {
            if (groupBar.isCanceled())
            {
                return createErrorResponse("Canceled", "Splitting clusters was canceled");
            }
            clustersById[faceValueIndices[triIdx]].push_back(triIdx);
            groupBar.addProgress(1);
        }
    }
    bar.addProgress(1);

    createdAssets.clear();
    nlohmann::json createdGuids = nlohmann::json::array();

    {
        CADacomb::ProgressBar extractBar(clustersById.size(), "Creating cluster assets");
        for (const auto& [clusterId, triIndices] : clustersById)
        {
            if (extractBar.isCanceled())
            {
                for (const auto& created : createdAssets)
                {
                    project.removeAsset(created->getGuid());
                }
                createdAssets.clear();
                return createErrorResponse("Canceled", "Splitting clusters was canceled");
            }

            std::unique_ptr<CADacomb::Triangulation> subTri = std::make_unique<CADacomb::Triangulation>();
            std::unordered_map<size_t, size_t>       indexMap;

            for (size_t triIdx : triIndices)
            {
                size_t startOffset = triIdx * 3;
                for (size_t i = 0; i < 3; i++)
                {
                    size_t oldVertexIdx = tri.indices[startOffset + i];
                    auto   it           = indexMap.find(oldVertexIdx);
                    if (it == indexMap.end())
                    {
                        size_t newIdx = subTri->vertices.size();
                        subTri->vertices.push_back(tri.vertices[oldVertexIdx]);
                        indexMap[oldVertexIdx] = newIdx;
                        subTri->indices.push_back(newIdx);
                    }
                    else
                    {
                        subTri->indices.push_back(it->second);
                    }
                }
            }

            std::shared_ptr<TriangulationAsset> newAsset = std::make_shared<TriangulationAsset>();
            newAsset->setName(triAsset->getName() + "_" + std::to_string(clusterId));
            newAsset->setTriangulation(std::move(subTri));
            project.addAsset(newAsset);

            createdGuids.push_back(newAsset->getGuid().toString());
            createdAssets.push_back(std::move(newAsset));
            extractBar.addProgress(1);
        }
    }
    bar.addProgress(1);

    return createResponse({
      {         "assetGuid",                      assetGuid.toString() },
      {     "componentGuid",                  componentGuid.toString() },
      { "createdAssetGuids",                              createdGuids },
      {      "createdCount", static_cast<int>(createdAssets.size()) }
    });
}

bool SplitClustersToAssetsCommand::isUndoable() const
{
    return true;
}

void SplitClustersToAssetsCommand::undo(Database& database)
{
    Project& project = database.getProject();
    for (const auto& created : createdAssets)
    {
        if (created)
        {
            project.removeAsset(created->getGuid());
        }
    }
}

void SplitClustersToAssetsCommand::redo(Database& database)
{
    Project& project = database.getProject();
    for (const auto& created : createdAssets)
    {
        if (created)
        {
            project.addAsset(created);
        }
    }
}

size_t SplitClustersToAssetsCommand::getUndoSize() const
{
    size_t total = sizeof(*this);
    for (const auto& created : createdAssets)
    {
        if (created)
        {
            total += created->getSizeInBytes();
        }
    }
    return total;
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("SplitClustersToAssetsCommand/SplitsTwoClustersIntoSeparateAssets")
{
    Database                                 db;
    std::unique_ptr<CADacomb::Triangulation> tri = std::make_unique<CADacomb::Triangulation>();
    tri->vertices                                = {
        glm::dvec3(0.0, 0.0, 0.0),
        glm::dvec3(1.0, 0.0, 0.0),
        glm::dvec3(0.0, 1.0, 0.0),
        glm::dvec3(5.0, 0.0, 0.0),
        glm::dvec3(6.0, 0.0, 0.0),
        glm::dvec3(5.0, 1.0, 0.0)
    };
    tri->indices = { 0, 1, 2, 3, 4, 5 };

    auto triAsset = std::make_shared<TriangulationAsset>();
    triAsset->setName("TwoTris");
    triAsset->setTriangulation(std::move(tri));
    db.getProject().addAsset(triAsset);

    auto comp = std::make_shared<FaceValuesComponent>();
    comp->setValues({
      { { "cluster", 0 } },
      { { "cluster", 1 } }
    });
    comp->setFaceValueIndices({ 0, 1 });
    CADacomb::GUID compGuid = triAsset->addComponent(comp);

    SplitClustersToAssetsCommand cmd;
    cmd.setInput({
      {          "type",        "SplitClustersToAssets" },
      {       "version",                              1 },
      {     "assetGuid", triAsset->getGuid().toString() },
      { "componentGuid",            compGuid.toString() }
    });

    nlohmann::json result = cmd.execute(db);
    CHECK(result.at("type") == "SplitClustersToAssets");
    CHECK(result.at("createdCount") == 2);
    CHECK(db.getProject().getAssets().size() == 3);

    auto firstSplit = std::dynamic_pointer_cast<TriangulationAsset>(db.getProject().getAsset(CADacomb::GUID(result.at("createdAssetGuids")[0].get<std::string>())));
    REQUIRE(firstSplit != nullptr);
    CHECK(firstSplit->getTriangulation().vertices.size() == 3);
    CHECK(firstSplit->getTriangulation().indices.size() == 3);

    cmd.undo(db);
    CHECK(db.getProject().getAssets().size() == 1);

    cmd.redo(db);
    CHECK(db.getProject().getAssets().size() == 3);
}

TEST_CASE("SplitClustersToAssetsCommand/UsesFirstFaceValuesComponentWhenOmitted")
{
    Database                                 db;
    std::unique_ptr<CADacomb::Triangulation> tri = std::make_unique<CADacomb::Triangulation>();
    tri->vertices                                = {
        glm::dvec3(0.0, 0.0, 0.0),
        glm::dvec3(1.0, 0.0, 0.0),
        glm::dvec3(0.0, 1.0, 0.0)
    };
    tri->indices = { 0, 1, 2 };

    auto triAsset = std::make_shared<TriangulationAsset>();
    triAsset->setName("SingleTri");
    triAsset->setTriangulation(std::move(tri));
    db.getProject().addAsset(triAsset);

    auto comp = std::make_shared<FaceValuesComponent>();
    comp->setValues({ { { "cluster", 0 } } });
    comp->setFaceValueIndices({ 0 });
    triAsset->addComponent(comp);

    SplitClustersToAssetsCommand cmd;
    cmd.setInput({
      {      "type",        "SplitClustersToAssets" },
      {   "version",                              1 },
      { "assetGuid", triAsset->getGuid().toString() }
    });

    nlohmann::json result = cmd.execute(db);
    CHECK(result.at("type") == "SplitClustersToAssets");
    CHECK(result.at("createdCount") == 1);
    CHECK(db.getProject().getAssets().size() == 2);
}
#endif
