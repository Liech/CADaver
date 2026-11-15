#include "Commands/Geometry/ClusterByNormalCommand.h"

#include "CADacomb/Triangle/Clustering.h"
#include "CADacomb/Triangle/Triangulation.h"
#include "CADacomb/Util/ProgressBar.h"
#include "Database/Asset/TriangulationAsset.h"
#include "Database/Component/FaceValuesComponent.h"
#include "Database/Database.h"

#include <cmath>
#include <glm/trigonometric.hpp>
#include <stdexcept>

ClusterByNormalCommand::ClusterByNormalCommand()
{
}

ClusterByNormalCommand::~ClusterByNormalCommand()
{
}

std::string ClusterByNormalCommand::getType() const
{
    return "ClusterByNormal";
}

int ClusterByNormalCommand::getVersion() const
{
    return 1;
}

std::string ClusterByNormalCommand::getDescription() const
{
    return "Command: ClusterByNormal\n"
           "Version: 1\n"
           "Purpose: Clusters faces of a Triangulation asset by normal angle threshold and attaches a FaceValuesComponent.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"ClusterByNormal\",\n"
           "  \"version\": 1,\n"
           "  \"assetGuid\": <string>,\n"
           "  \"maxAngleDegrees\": <number> (optional, default 35.0),\n"
           "  \"withoutHoles\": <bool> (optional, default false),\n"
           "  \"name\": <string> (optional, default \"Normal Clusters\")\n"
           "}\n\n"
           "Output (JSON):\n"
           "{\n"
           "  \"type\": \"ClusterByNormal\",\n"
           "  \"version\": 1,\n"
           "  \"assetGuid\": <string>,\n"
           "  \"componentGuid\": <string>,\n"
           "  \"clusterCount\": <integer>\n"
           "}";
}

std::vector<CADacomb::GUID> ClusterByNormalCommand::getRequiredAssets() const
{
    return { assetGuid };
}

void ClusterByNormalCommand::setInput(const Polyglot::Node& node)
{
    assetGuid       = CADacomb::GUID(node["assetGuid"].asString());
    maxAngleDegrees = 35.0;
    withoutHoles    = false;
    componentName   = "Normal Clusters";

    if (node.contains("maxAngleDegrees") && !node["maxAngleDegrees"].isNull())
    {
        maxAngleDegrees = node["maxAngleDegrees"].asDouble();
    }
    if (node.contains("withoutHoles") && !node["withoutHoles"].isNull())
    {
        withoutHoles = node["withoutHoles"].asBool();
    }
    if (node.contains("name") && !node["name"].isNull())
    {
        componentName = node["name"].asString();
    }
}

nlohmann::json ClusterByNormalCommand::execute(Database& database)
{
    CADacomb::ProgressBar bar(2, "Clustering faces by normal");

    Project&               project = database.getProject();
    std::shared_ptr<Asset> asset   = project.getAsset(assetGuid);
    if (!asset)
    {
        return createErrorResponse("AssetNotFound", "Asset not found: " + assetGuid.toString());
    }

    std::shared_ptr<TriangulationAsset> triAsset = std::dynamic_pointer_cast<TriangulationAsset>(asset);
    if (!triAsset)
    {
        return createErrorResponse("InvalidAssetType", "Asset is not a TriangulationAsset: " + assetGuid.toString());
    }

    const CADacomb::Triangulation& tri                = triAsset->getTriangulation();
    double                         normalDotThreshold = std::cos(glm::radians(maxAngleDegrees));

    std::vector<std::vector<size_t>> clusters;
    try
    {
        clusters = CADacomb::Clustering::clusterByNormal(tri, normalDotThreshold, withoutHoles);
    }
    catch (const std::runtime_error& error)
    {
        return createErrorResponse("UnhealthyMesh", error.what());
    }

    if (bar.isCanceled())
    {
        return createErrorResponse("Canceled", "Normal clustering was canceled");
    }
    bar.addProgress(1);

    std::vector<nlohmann::json> values;
    values.reserve(clusters.size());

    std::vector<uint32_t> faceValueIndices(tri.indices.size() / 3, 0);
    {
        CADacomb::ProgressBar assignBar(clusters.size(), "Assigning cluster values");
        for (size_t i = 0; i < clusters.size(); i++)
        {
            if (assignBar.isCanceled())
            {
                return createErrorResponse("Canceled", "Normal clustering was canceled");
            }
            values.push_back({
              {   "cluster", static_cast<int>(i) },
              { "faceCount", static_cast<int>(clusters[i].size()) }
            });

            for (size_t faceIndex : clusters[i])
            {
                faceValueIndices[faceIndex] = static_cast<uint32_t>(i);
            }
            assignBar.addProgress(1);
        }
    }
    bar.addProgress(1);

    nlohmann::json metadata = {
        { "maxAngleDegrees", maxAngleDegrees },
        { "withoutHoles", withoutHoles },
        { "clusterCount", static_cast<int>(clusters.size()) }
    };

    addedComponent = std::make_shared<FaceValuesComponent>();
    addedComponent->setName(componentName);
    addedComponent->setValues(values);
    addedComponent->setFaceValueIndices(faceValueIndices);
    addedComponent->setMetadata(metadata);
    addedComponent->setLuaScript(FaceValuesComponent::getDefaultLuaScript());

    addedComponentGuid = addedComponent->getGuid();
    previousSelection  = database.getSelection().getEntries();

    triAsset->addComponent(addedComponent);
    SelectionEntry entry;
    entry.setTargetGuid(addedComponentGuid);
    database.getSelection().select(entry);

    return createResponse({
      { "assetGuid", assetGuid.toString() },
      { "componentGuid", addedComponentGuid.toString() },
      { "clusterCount", static_cast<int>(clusters.size()) }
    });
}

bool ClusterByNormalCommand::isUndoable() const
{
    return true;
}

void ClusterByNormalCommand::undo(Database& database)
{
    Project&                            project  = database.getProject();
    std::shared_ptr<TriangulationAsset> triAsset = std::dynamic_pointer_cast<TriangulationAsset>(project.getAsset(assetGuid));
    if (triAsset)
    {
        triAsset->removeComponent(addedComponentGuid);
    }
    database.getSelection().clear();
    for (const SelectionEntry& entry : previousSelection)
    {
        database.getSelection().addSelection(entry);
    }
}

void ClusterByNormalCommand::redo(Database& database)
{
    Project&                            project  = database.getProject();
    std::shared_ptr<TriangulationAsset> triAsset = std::dynamic_pointer_cast<TriangulationAsset>(project.getAsset(assetGuid));
    if (triAsset && addedComponent)
    {
        triAsset->addComponent(addedComponent);
        SelectionEntry entry;
        entry.setTargetGuid(addedComponentGuid);
        database.getSelection().select(entry);
    }
}

size_t ClusterByNormalCommand::getUndoSize() const
{
    if (addedComponent)
    {
        return sizeof(*this) + addedComponent->getSizeInBytes();
    }
    return sizeof(*this);
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("ClusterByNormalCommand/SplitsPerpendicularFaces")
{
    Database                db;
    std::unique_ptr<CADacomb::Triangulation> tri = std::make_unique<CADacomb::Triangulation>();
    tri->vertices                                = {
        glm::dvec3(0.0, 0.0, 0.0),
        glm::dvec3(1.0, 0.0, 0.0),
        glm::dvec3(1.0, 1.0, 0.0),
        glm::dvec3(0.0, 1.0, 0.0),
        glm::dvec3(1.0, 0.0, 1.0)
    };
    tri->indices = {
        0, 1, 2,
        0, 2, 3,
        1, 4, 2
    };

    std::shared_ptr<TriangulationAsset> triAsset = std::make_shared<TriangulationAsset>();
    triAsset->setTriangulation(std::move(tri));
    triAsset->setName("LShape");
    db.getProject().addAsset(triAsset);

    ClusterByNormalCommand cmd;
    Polyglot::Node         input = {
        {            "type",             "ClusterByNormal" },
        {         "version",                             1 },
        {       "assetGuid", triAsset->getGuid().toString() },
        { "maxAngleDegrees",                          10.0 },
        {    "withoutHoles",                         false }
    };
    cmd.setInput(input);

    nlohmann::json result = cmd.execute(db);
    CHECK(result["type"] == "ClusterByNormal");
    CHECK(result["clusterCount"] == 2);
    CHECK(triAsset->getAttachments().size() == 1);
    CHECK(db.getSelection().isSelected(CADacomb::GUID(result["componentGuid"].get<std::string>())));

    cmd.undo(db);
    CHECK(triAsset->getAttachments().empty());

    cmd.redo(db);
    CHECK(triAsset->getAttachments().size() == 1);
}

TEST_CASE("ClusterByNormalCommand/WithoutHolesSplitsHoledMesh")
{
    Database                                 db;
    std::unique_ptr<CADacomb::Triangulation> tri = std::make_unique<CADacomb::Triangulation>();
    for (int y = 0; y < 4; y++)
    {
        for (int x = 0; x < 4; x++)
        {
            tri->vertices.push_back(glm::dvec3(x, y, 0.0));
        }
    }
    for (int y = 0; y < 3; y++)
    {
        for (int x = 0; x < 3; x++)
        {
            if (x == 1 && y == 1)
            {
                continue;
            }
            size_t a = y * 4 + x;
            size_t b = y * 4 + x + 1;
            size_t c = (y + 1) * 4 + x + 1;
            size_t d = (y + 1) * 4 + x;
            tri->indices.insert(tri->indices.end(), { a, b, c, a, c, d });
        }
    }

    std::shared_ptr<TriangulationAsset> triAsset = std::make_shared<TriangulationAsset>();
    triAsset->setTriangulation(std::move(tri));
    triAsset->setName("HoledGrid");
    db.getProject().addAsset(triAsset);

    ClusterByNormalCommand cmd;
    Polyglot::Node         input = {
        {            "type",             "ClusterByNormal" },
        {         "version",                             1 },
        {       "assetGuid", triAsset->getGuid().toString() },
        { "maxAngleDegrees",                          30.0 },
        {    "withoutHoles",                          true }
    };
    cmd.setInput(input);

    nlohmann::json result = cmd.execute(db);
    CHECK(result["type"] == "ClusterByNormal");
    CHECK(result["clusterCount"].get<int>() >= 2);
}
#endif
