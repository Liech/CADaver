#include "Commands/IO/LoadAssetCommand.h"

#include "Database/Asset.h"
#include "Database/Database.h"
#include "IO/AssetLoader/AssetLoader.h"
#include "IO/AssetLoader/AssetLoaderFactory.h"

LoadAssetCommand::LoadAssetCommand() {}

LoadAssetCommand::~LoadAssetCommand() {}

std::string LoadAssetCommand::getType() const
{
    return "LoadAsset";
}

int LoadAssetCommand::getVersion() const
{
    return 1;
}

std::string LoadAssetCommand::getDescription() const
{
    return "Command: LoadAsset\n"
           "Version: 1\n"
           "Purpose: Loads a CAD (STEP), Triangulation (STL), or Voxel (SVO) asset from a source callback or file path into the project.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"LoadAsset\",\n"
           "  \"version\": 1,\n"
           "  \"name\": \"<string>\",\n"
           "  \"assetType\": \"Cad\" | \"Triangulation\" | \"Voxel\" (optional if inferrable from file extension),\n"
           "  \"source\": <callback or string>\n"
           "}\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"LoadAsset\",\n"
           "  \"version\": 1,\n"
           "  \"status\": \"Success\" | \"UnsupportedType\" | \"LoadFailed\",\n"
           "  \"assetGuid\": \"<string>\",\n"
           "  \"assetType\": \"<string>\",\n"
           "  \"name\": \"<string>\"\n"
           "}";
}

void LoadAssetCommand::setInput(const Polyglot::Node& node)
{
    name   = node["name"].asString();
    source = node["source"];
    if (node.contains("assetType") && node["assetType"].isString())
    {
        assetType = node["assetType"].asString();
    }
    else
    {
        assetType.clear();
    }
}

nlohmann::json LoadAssetCommand::execute(Database& database)
{
    AssetLoaderFactory factory;
    std::string        resolvedType = factory.resolveAssetType(assetType, source, name);
    auto               loader       = factory.create(resolvedType);
    if (!loader)
    {
        return createResponse("UnsupportedType",
                              {
                                { "assetGuid",           "" },
                                { "assetType", resolvedType },
                                {      "name",         name }
        });
    }

    auto asset = loader->load(name, source);
    if (!asset)
    {
        return createResponse("LoadFailed",
                              {
                                { "assetGuid",           "" },
                                { "assetType", resolvedType },
                                {      "name",         name }
        });
    }

    database.getProject().addAsset(asset);
    createdAsset = asset;

    return createResponse("Success",
                          {
                            { "assetGuid", asset->getGuid().toString() },
                            { "assetType",            asset->getType() },
                            {      "name",                        name }
    });
}

bool LoadAssetCommand::isUndoable() const
{
    return createdAsset != nullptr;
}

void LoadAssetCommand::undo(Database& database)
{
    if (createdAsset)
    {
        database.getProject().removeAsset(createdAsset->getGuid());
    }
}

void LoadAssetCommand::redo(Database& database)
{
    if (createdAsset)
    {
        database.getProject().addAsset(createdAsset);
    }
}

size_t LoadAssetCommand::getUndoSize() const
{
    return sizeof(LoadAssetCommand) + (createdAsset ? createdAsset->getSizeInBytes() : 0);
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("LoadAssetCommand/TypeAndVersion")
{
    LoadAssetCommand cmd;
    cmd.setInput(Polyglot::Node({
      {      "name",          "Mesh" },
      { "assetType", "Triangulation" },
      {    "source",      "mesh.stl" }
    }));
    CHECK(cmd.getType() == "LoadAsset");
    CHECK(cmd.getVersion() == 1);
    CHECK_FALSE(cmd.getDescription().empty());
    CHECK(cmd.getRequiredAssets().empty());
}
#endif
