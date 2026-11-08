#include "Commands/IO/LoadStlAssetCommand.h"

#include "CADacomb/Triangle/Triangulation.h"
#include "CADacomb/Util/ProgressBar.h"
#include "Database/Asset/TriangulationAsset.h"
#include "Database/Database.h"
#include "Utility/IOValueGrabber.h"

#include <sstream>

LoadStlAssetCommand::LoadStlAssetCommand() {}

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
           "Purpose: Loads an STL triangulation asset from a source callback or file path into the project.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"LoadStlAsset\",\n"
           "  \"version\": 1,\n"
           "  \"name\": \"<string>\",\n"
           "  \"source\": <callback or string>\n"
           "}\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"LoadStlAsset\",\n"
           "  \"version\": 1,\n"
           "  \"status\": \"Success\" | \"ParseFailed\",\n"
           "  \"index\": <int>,\n"
           "  \"name\": \"<string>\"\n"
           "}";
}

void LoadStlAssetCommand::setInput(const Polyglot::Node& node)
{
    name   = node["name"].asString();
    source = node["source"];
}

nlohmann::json LoadStlAssetCommand::execute(Database& database)
{
    CADacomb::ProgressBar bar(3, "Loading STL asset");

    std::unique_ptr<CADacomb::Triangulation> triangulation;

    std::vector<uint8_t> bytes = IOValueGrabber::grabBytes(source);
    bar.addProgress(1);

    if (!bytes.empty())
    {
        std::string        str(bytes.begin(), bytes.end());
        std::istringstream stream(str, std::ios::binary);
        triangulation = CADacomb::Triangulation::fromSTLStream(stream);
    }
    bar.addProgress(1);

    if (!triangulation || triangulation->indices.empty() || bar.isCanceled())
    {
        return createResponse("ParseFailed",
                              {
                                { "index",   -1 },
                                {  "name", name }
        });
    }

    auto asset = std::make_shared<TriangulationAsset>();
    asset->setName(name);
    asset->setTriangulation(std::move(triangulation));
    database.getProject().addAsset(asset);
    createdAsset = asset;

    bar.addProgress(1);

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

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("LoadStlAssetCommand/TypeAndVersion")
{
    LoadStlAssetCommand cmd;
    cmd.setInput(Polyglot::Node({
      {   "name",  "Mesh" },
      { "source", "dummy" }
    }));
    CHECK(cmd.getType() == "LoadStlAsset");
    CHECK(cmd.getVersion() == 1);
    CHECK_FALSE(cmd.getDescription().empty());
    CHECK(cmd.getRequiredAssets().empty());
}
#endif
