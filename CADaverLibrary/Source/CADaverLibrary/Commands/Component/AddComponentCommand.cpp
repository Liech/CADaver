#include "Commands/Component/AddComponentCommand.h"

#include "Database/Asset.h"
#include "Database/AssetComponent.h"
#include "Database/Component/ComponentFactory.h"
#include "Database/Database.h"
#include "Database/Project.h"

AddComponentCommand::AddComponentCommand() {}

AddComponentCommand::~AddComponentCommand() {}

std::string AddComponentCommand::getType() const
{
    return "AddComponent";
}

int AddComponentCommand::getVersion() const
{
    return 1;
}

std::string AddComponentCommand::getDescription() const
{
    return "Command: AddComponent\n"
           "Version: 1\n"
           "Purpose: Creates and attaches a registered component (e.g. TextNote, FaceValues) to an asset.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"AddComponent\",\n"
           "  \"version\": 1,\n"
           "  \"assetGuid\": \"<string>\",\n"
           "  \"componentType\": \"<string>\",\n"
           "  \"name\": \"<string, optional>\",\n"
           "  \"data\": <object, optional>\n"
           "}\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"AddComponent\",\n"
           "  \"version\": 1,\n"
           "  \"componentGuid\": \"<string>\"\n"
           "}";
}

void AddComponentCommand::setInput(const Polyglot::Node& node)
{
    assetGuid     = CADacomb::GUID(node["assetGuid"].asString());
    componentType = node["componentType"].asString();
    name          = node.contains("name") ? node["name"].asString() : "";
    hasData       = node.contains("data") && !node["data"].isNull();
    if (hasData)
    {
        data = node["data"].toJson();
    }
}

nlohmann::json AddComponentCommand::execute(Database& database)
{
    auto asset = database.getProject().getAsset(assetGuid);
    if (!asset)
    {
        return createErrorResponse("AssetNotFound", "Asset not found: " + assetGuid.toString());
    }

    ComponentFactory factory;
    addedComponent = factory.create(componentType);
    if (!addedComponent)
    {
        return createErrorResponse("UnknownComponentType", "Unknown component type: " + componentType);
    }

    if (!addedComponent->isCompatible(asset->getType()))
    {
        return createErrorResponse("IncompatibleComponent", "Component " + componentType + " is incompatible with asset type " + asset->getType());
    }

    if (!name.empty())
    {
        addedComponent->setName(name);
    }

    if (hasData)
    {
        addedComponent->fromDataJson(data);
    }

    CADacomb::GUID compGuid = asset->addComponent(addedComponent);
    return createResponse({
        { "componentGuid", compGuid.toString() }
    });
}

bool AddComponentCommand::isUndoable() const
{
    return true;
}

void AddComponentCommand::undo(Database& database)
{
    if (addedComponent)
    {
        auto asset = database.getProject().getAsset(assetGuid);
        if (asset)
        {
            asset->removeComponent(addedComponent->getGuid());
        }
    }
}

void AddComponentCommand::redo(Database& database)
{
    if (addedComponent)
    {
        auto asset = database.getProject().getAsset(assetGuid);
        if (asset)
        {
            asset->addComponent(addedComponent);
        }
    }
}

size_t AddComponentCommand::getUndoSize() const
{
    return sizeof(AddComponentCommand) + (addedComponent ? addedComponent->getSizeInBytes() : 0);
}

#ifdef ISTESTPROJECT
#include "Database/Asset/CadAsset.h"
#include "Database/Asset/TriangulationAsset.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("AddComponentCommand/AddsCompatibleComponentAndSupportsUndoRedo")
{
    Database database;
    auto     triAsset = std::make_shared<TriangulationAsset>();
    database.getProject().addAsset(triAsset);

    AddComponentCommand cmd;
    cmd.setInput(Polyglot::Node({
      {     "assetGuid", triAsset->getGuid().toString() },
      { "componentType",                   "FaceValues" },
      {          "name",                     "Clusters" }
    }));

    nlohmann::json res = cmd.execute(database);
    CHECK(res.at("type").get<std::string>() == "AddComponent");
    CADacomb::GUID compGuid(res.at("componentGuid").get<std::string>());
    REQUIRE(triAsset->getComponent(compGuid) != nullptr);
    CHECK(triAsset->getComponent(compGuid)->getName() == "Clusters");

    cmd.undo(database);
    CHECK(triAsset->getComponent(compGuid) == nullptr);

    cmd.redo(database);
    CHECK(triAsset->getComponent(compGuid) != nullptr);
}

TEST_CASE("AddComponentCommand/RejectsIncompatibleAssetType")
{
    Database database;
    auto     cadAsset = std::make_shared<CadAsset>();
    database.getProject().addAsset(cadAsset);

    AddComponentCommand cmd;
    cmd.setInput(Polyglot::Node({
      {     "assetGuid", cadAsset->getGuid().toString() },
      { "componentType",                   "FaceValues" }
    }));

    nlohmann::json res = cmd.execute(database);
    CHECK(res.at("type").get<std::string>() == "Error");
    CHECK(res.at("errorType").get<std::string>() == "IncompatibleComponent");
}
#endif
