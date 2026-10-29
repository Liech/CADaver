#include "Commands/IO/LoadStlAssetCommand.h"

#include "CADacomb/Triangle/Triangulation.h"
#include "CADacomb/Util/ProgressBar.h"
#include "Commands/Undo/AddAssetUndoRecord.h"
#include "Database/Asset/TriangulationAsset.h"
#include "Database/Database.h"

#include <fstream>

LoadStlAssetCommand::LoadStlAssetCommand() {}

LoadStlAssetCommand::LoadStlAssetCommand(const std::string& name, const std::string& filePath)
  : name(name)
  , filePath(filePath)
{
}

LoadStlAssetCommand::LoadStlAssetCommand(const nlohmann::json& json)
  : name(json.at("name").get<std::string>())
  , filePath(json.value("filePath", ""))
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
           "Purpose: Loads an STL triangulation asset from an input stream or file path into the project.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"LoadStlAsset\",\n"
           "  \"version\": 1,\n"
           "  \"name\": \"<string>\",\n"
           "  \"filePath\": \"<string>\" (optional)\n"
           "}\n\n"
           "Input Stream: Binary or ASCII STL stream (optional if filePath specified)\n"
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

nlohmann::json LoadStlAssetCommand::execute(Database& database, std::istream* inputStream, std::ostream*, bool recordUndo)
{
    CADacomb::ProgressBar bar(1, "Loading STL asset");

    std::ifstream fileStream;
    std::istream* streamToUse = inputStream;
    if (!streamToUse && !filePath.empty())
    {
        fileStream.open(filePath, std::ios::binary);
        if (fileStream.is_open())
        {
            streamToUse = &fileStream;
        }
    }

    if (!streamToUse || bar.isCanceled())
    {
        return createResponse("NoInputStream",
                              {
                                { "index",   -1 },
                                {  "name", name }
        });
    }

    auto triangulation = CADacomb::Triangulation::fromSTLStream(*streamToUse);
    if (!triangulation || triangulation->indices.empty() || bar.isCanceled())
    {
        return createResponse("ParseFailed",
                              {
                                { "index",   -1 },
                                {  "name", name }
        });
    }

    bar.addProgress(1);

    auto asset = std::make_shared<TriangulationAsset>(name, std::move(triangulation));
    database.getProject().addAsset(asset);
    database.getProject().setSelectedAssetGuid(asset->getGuid());

    if (recordUndo)
    {
        database.getUndoStack().push(std::make_unique<AddAssetUndoRecord>(asset));
    }

    return createResponse("Success",
                          {
                            { "assetGuid", asset->getGuid().toString() },
                            {      "name",                        name }
    });
}

const std::string& LoadStlAssetCommand::getName() const
{
    return name;
}

void LoadStlAssetCommand::setName(const std::string& newName)
{
    name = newName;
}

const std::string& LoadStlAssetCommand::getFilePath() const
{
    return filePath;
}

void LoadStlAssetCommand::setFilePath(const std::string& newFilePath)
{
    filePath = newFilePath;
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("LoadStlAssetCommand/TypeAndVersion")
{
    LoadStlAssetCommand cmd("Mesh", "mesh.stl");
    CHECK(cmd.getType() == "LoadStlAsset");
    CHECK(cmd.getVersion() == 1);
    CHECK_FALSE(cmd.getDescription().empty());
    CHECK(cmd.getName() == "Mesh");
    CHECK(cmd.getFilePath() == "mesh.stl");
    CHECK(cmd.getRequiredAssets().empty());
}
#endif
