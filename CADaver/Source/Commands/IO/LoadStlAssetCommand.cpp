#include "Commands/IO/LoadStlAssetCommand.h"

#include "CADacomb/Triangle/Triangulation.h"
#include "CADacomb/Util/ProgressBar.h"
#include "Database/Asset/TriangulationAsset.h"
#include "Database/Database.h"

LoadStlAssetCommand::LoadStlAssetCommand() {}

LoadStlAssetCommand::LoadStlAssetCommand(const std::string& name)
  : name(name)
{
}

LoadStlAssetCommand::LoadStlAssetCommand(const nlohmann::json& json)
  : name(json.at("name").get<std::string>())
{
}

LoadStlAssetCommand::~LoadStlAssetCommand() {}

std::string LoadStlAssetCommand::getType() const
{
    return "LoadStlAsset";
}

int LoadStlAssetCommand::getVersion() const
{
    return 1;
}

std::string LoadStlAssetCommand::getDescription() const
{
    return "Command: LoadStlAsset\n"
           "Version: 1\n"
           "Purpose: Loads an STL triangulation asset from an input stream into the project.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"LoadStlAsset\",\n"
           "  \"version\": 1,\n"
           "  \"name\": \"<string>\"\n"
           "}\n\n"
           "Input Stream: Binary or ASCII STL stream\n"
           "Output Stream: None\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"LoadStlAsset\",\n"
           "  \"version\": 1,\n"
           "  \"status\": \"Success\" | \"NoInputStream\" | \"ParseFailed\",\n"
           "  \"index\": <int>,\n"
           "  \"name\": \"<string>\"\n"
           "}";
}

void LoadStlAssetCommand::setInput(const nlohmann::json& json)
{
    name = json.at("name").get<std::string>();
}

nlohmann::json LoadStlAssetCommand::execute(Database& database, std::istream* inputStream, std::ostream*)
{
    CADacomb::ProgressBar bar(1, "Loading STL asset");

    if (!inputStream || bar.isCanceled())
    {
        return createResponse("NoInputStream",
                              {
                                { "index",   -1 },
                                {  "name", name }
        });
    }

    auto triangulation = CADacomb::Triangulation::fromSTLStream(*inputStream);
    if (!triangulation || triangulation->indices.empty() || bar.isCanceled())
    {
        return createResponse("ParseFailed",
                              {
                                { "index",   -1 },
                                {  "name", name }
        });
    }

    bar.addProgress(1);

    auto asset = std::make_shared<TriangulationAsset>();
    asset->setName(name);
    asset->setTriangulation(std::move(triangulation));
    database.getProject().addAsset(asset);
    createdAsset = asset;

    return createResponse("Success",
                          {
                            { "assetGuid", asset->getGuid().toString() },
                            {      "name",                        name }
    });
}

bool LoadStlAssetCommand::isUndoable() const
{
    return createdAsset != nullptr;
}

void LoadStlAssetCommand::undo(Database& database)
{
    if (createdAsset)
    {
        database.getProject().removeAsset(createdAsset->getGuid());
    }
}

void LoadStlAssetCommand::redo(Database& database)
{
    if (createdAsset)
    {
        database.getProject().addAsset(createdAsset);
    }
}

size_t LoadStlAssetCommand::getUndoSize() const
{
    return sizeof(LoadStlAssetCommand) + (createdAsset ? createdAsset->getSizeInBytes() : 0);
}

const std::string& LoadStlAssetCommand::getName() const
{
    return name;
}

void LoadStlAssetCommand::setName(const std::string& newName)
{
    name = newName;
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("LoadStlAssetCommand/TypeAndVersion")
{
    LoadStlAssetCommand cmd(std::string("Mesh"));
    CHECK(cmd.getType() == "LoadStlAsset");
    CHECK(cmd.getVersion() == 1);
    CHECK_FALSE(cmd.getDescription().empty());
    CHECK(cmd.getName() == "Mesh");
    CHECK(cmd.getRequiredAssets().empty());
}
#endif
