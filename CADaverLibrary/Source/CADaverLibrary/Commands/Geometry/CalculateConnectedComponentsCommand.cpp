#include "Commands/Geometry/CalculateConnectedComponentsCommand.h"

#include "CADacomb/Triangle/Clustering.h"
#include "CADacomb/Triangle/Triangulation.h"
#include "Database/Asset/TriangulationAsset.h"
#include "Database/Component/FaceValuesComponent.h"
#include "Database/Database.h"
#include "Database/Project.h"
#include "Database/Selection/SelectionEntry.h"

CalculateConnectedComponentsCommand::CalculateConnectedComponentsCommand() {}

CalculateConnectedComponentsCommand::~CalculateConnectedComponentsCommand() {}

std::string CalculateConnectedComponentsCommand::getType() const
{
    return "CalculateConnectedComponents";
}

int CalculateConnectedComponentsCommand::getVersion() const
{
    return 1;
}

std::string CalculateConnectedComponentsCommand::getDescription() const
{
    return "Command: CalculateConnectedComponents\n"
           "Version: 1\n"
           "Purpose: Computes connected components of a TriangulationAsset, attaches a FaceValuesComponent, and selects it for display.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"CalculateConnectedComponents\",\n"
           "  \"version\": 1,\n"
           "  \"assetGuid\": \"<string>\",\n"
           "  \"name\": \"<string, optional>\"\n"
           "}\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"CalculateConnectedComponents\",\n"
           "  \"version\": 1,\n"
           "  \"componentGuid\": \"<string>\",\n"
           "  \"componentCount\": <int>\n"
           "}";
}

void CalculateConnectedComponentsCommand::setInput(const Polyglot::Node& node)
{
    assetGuid = CADacomb::GUID(node["assetGuid"].asString());
    name      = node.contains("name") ? node["name"].asString() : "";
}

nlohmann::json CalculateConnectedComponentsCommand::execute(Database& database)
{
    auto asset = database.getProject().getAsset(assetGuid);
    if (!asset)
    {
        return createErrorResponse("AssetNotFound", "Asset not found: " + assetGuid.toString());
    }

    auto triAsset = std::dynamic_pointer_cast<TriangulationAsset>(asset);
    if (!triAsset)
    {
        return createErrorResponse("IncompatibleAsset", "Asset is not a Triangulation asset: " + assetGuid.toString());
    }

    const CADacomb::Triangulation&   tri          = triAsset->getTriangulation();
    size_t                           numTriangles = tri.indices.size() / 3;
    std::vector<std::vector<size_t>> patches      = CADacomb::Clustering::cluster(tri, [](size_t, size_t, const CADacomb::Triangulation&) { return true; });

    std::vector<nlohmann::json> values;
    values.reserve(patches.size());
    std::vector<uint32_t> faceValueIndices(numTriangles, 0);

    for (size_t patchIdx = 0; patchIdx < patches.size(); patchIdx++)
    {
        values.push_back({
          {   "cluster",            patchIdx },
          { "faceCount", patches[patchIdx].size() }
        });

        for (size_t faceIdx : patches[patchIdx])
        {
            if (faceIdx < numTriangles)
            {
                faceValueIndices[faceIdx] = static_cast<uint32_t>(patchIdx);
            }
        }
    }

    addedComponent = std::make_shared<FaceValuesComponent>();
    addedComponent->setName(name.empty() ? "Connected Components" : name);
    addedComponent->setValues(values);
    addedComponent->setFaceValueIndices(faceValueIndices);
    addedComponent->setMetadata({
      { "componentCount", patches.size() },
      {      "faceCount",   numTriangles }
    });

    CADacomb::GUID compGuid = triAsset->addComponent(addedComponent);

    previousSelection = database.getSelection();
    SelectionEntry entry;
    entry.setTargetGuid(compGuid);
    database.getSelection().select(entry);

    return createResponse({
      {  "componentGuid", compGuid.toString() },
      { "componentCount",      patches.size() }
    });
}

bool CalculateConnectedComponentsCommand::isUndoable() const
{
    return true;
}

void CalculateConnectedComponentsCommand::undo(Database& database)
{
    if (addedComponent)
    {
        auto asset = database.getProject().getAsset(assetGuid);
        if (asset)
        {
            asset->removeComponent(addedComponent->getGuid());
        }
        database.getSelection() = previousSelection;
    }
}

void CalculateConnectedComponentsCommand::redo(Database& database)
{
    if (addedComponent)
    {
        auto asset = database.getProject().getAsset(assetGuid);
        if (asset)
        {
            asset->addComponent(addedComponent);
            SelectionEntry entry;
            entry.setTargetGuid(addedComponent->getGuid());
            database.getSelection().select(entry);
        }
    }
}

size_t CalculateConnectedComponentsCommand::getUndoSize() const
{
    return sizeof(CalculateConnectedComponentsCommand) + (addedComponent ? addedComponent->getSizeInBytes() : 0);
}

#ifdef ISTESTPROJECT
#include "Database/Asset/CadAsset.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("CalculateConnectedComponentsCommand/FindsDisconnectedTrianglesAndSelectsComponent")
{
    Database database;
    auto     tri = std::make_unique<CADacomb::Triangulation>();
    tri->vertices = {
        { 0.0, 0.0, 0.0 },
        { 1.0, 0.0, 0.0 },
        { 0.0, 1.0, 0.0 },
        { 1.0, 1.0, 0.0 },
        { 5.0, 0.0, 0.0 },
        { 6.0, 0.0, 0.0 },
        { 5.0, 1.0, 0.0 }
    };
    tri->indices = {
        0, 1, 2,
        1, 3, 2,
        4, 5, 6
    };

    auto triAsset = std::make_shared<TriangulationAsset>();
    triAsset->setTriangulation(std::move(tri));
    database.getProject().addAsset(triAsset);

    CalculateConnectedComponentsCommand cmd;
    cmd.setInput(Polyglot::Node({
      { "assetGuid", triAsset->getGuid().toString() }
    }));

    nlohmann::json res = cmd.execute(database);
    CHECK(res.at("type").get<std::string>() == "CalculateConnectedComponents");
    CHECK(res.at("componentCount").get<size_t>() == 2);

    CADacomb::GUID compGuid(res.at("componentGuid").get<std::string>());
    auto           comp = std::dynamic_pointer_cast<FaceValuesComponent>(triAsset->getComponent(compGuid));
    REQUIRE(comp != nullptr);
    CHECK(comp->getValues().size() == 2);
    CHECK(comp->getFaceValueIndices() == std::vector<uint32_t>{ 0, 0, 1 });
    CHECK(database.getSelection().isSelected(compGuid) == true);

    cmd.undo(database);
    CHECK(triAsset->getComponent(compGuid) == nullptr);
    CHECK(database.getSelection().isSelected(compGuid) == false);

    cmd.redo(database);
    CHECK(triAsset->getComponent(compGuid) != nullptr);
    CHECK(database.getSelection().isSelected(compGuid) == true);
}

TEST_CASE("CalculateConnectedComponentsCommand/RejectsNonTriangulationAsset")
{
    Database database;
    auto     cadAsset = std::make_shared<CadAsset>();
    database.getProject().addAsset(cadAsset);

    CalculateConnectedComponentsCommand cmd;
    cmd.setInput(Polyglot::Node({
      { "assetGuid", cadAsset->getGuid().toString() }
    }));

    nlohmann::json res = cmd.execute(database);
    CHECK(res.at("type").get<std::string>() == "Error");
    CHECK(res.at("errorType").get<std::string>() == "IncompatibleAsset");
}
#endif
