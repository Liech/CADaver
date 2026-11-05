#include "Commands/IO/LoadSvoAssetCommand.h"

#include "CADacomb/Util/ProgressBar.h"
#include "CADacomb/Volume/SVO/SVOFile.h"
#include "Database/Asset/VoxelAsset.h"
#include "Database/Database.h"
#include "Utility/IOValueGrabber.h"

#include <sstream>

LoadSvoAssetCommand::LoadSvoAssetCommand() {}

LoadSvoAssetCommand::~LoadSvoAssetCommand() {}

std::string LoadSvoAssetCommand::getType() const
{
    return "LoadSvoAsset";
}

int LoadSvoAssetCommand::getVersion() const
{
    return 1;
}

std::string LoadSvoAssetCommand::getDescription() const
{
    return "Command: LoadSvoAsset\n"
           "Version: 1\n"
           "Purpose: Loads an SVO voxel asset from a source callback or file path into the project.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"LoadSvoAsset\",\n"
           "  \"version\": 1,\n"
           "  \"name\": \"<string>\",\n"
           "  \"source\": <callback or string>\n"
           "}\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"LoadSvoAsset\",\n"
           "  \"version\": 1,\n"
           "  \"status\": \"Success\" | \"ParseFailed\",\n"
           "  \"index\": <int>,\n"
           "  \"name\": \"<string>\"\n"
           "}";
}

void LoadSvoAssetCommand::setInput(const Polyglot::Node& node)
{
    auto& mutableNode = const_cast<Polyglot::Node&>(node);
    name              = static_cast<std::string>(mutableNode["name"]);
    source            = mutableNode["source"];
}

nlohmann::json LoadSvoAssetCommand::execute(Database& database)
{
    CADacomb::ProgressBar bar(1, "Loading SVO asset");

    CADacomb::AABB3                      bounds;
    std::unique_ptr<CADacomb::SVO<bool>> svo;

    std::vector<uint8_t> bytes = IOValueGrabber::grabBytes(source);
    if (!bytes.empty())
    {
        std::string        str(reinterpret_cast<const char*>(bytes.data()), bytes.size());
        std::istringstream stream(str, std::ios::binary);
        svo = CADacomb::SVOFile::load<bool>(stream, bounds);
    }

    if (!svo || bar.isCanceled())
    {
        return createResponse("ParseFailed",
                              {
                                { "index",   -1 },
                                {  "name", name }
        });
    }

    if (bounds.getSize().x <= 0.0 || bounds.getSize().y <= 0.0 || bounds.getSize().z <= 0.0)
    {
        bounds = CADacomb::AABB3(glm::dvec3(0.0), glm::dvec3(static_cast<double>(svo->getDimension())));
    }

    bar.addProgress(1);

    auto asset = std::make_shared<VoxelAsset>();
    asset->setName(name);
    asset->setSVO(std::move(svo));
    asset->setAABB(bounds);
    database.getProject().addAsset(asset);
    createdAsset = asset;

    return createResponse("Success",
                          {
                            { "assetGuid", asset->getGuid().toString() },
                            {      "name",                        name }
    });
}

bool LoadSvoAssetCommand::isUndoable() const
{
    return createdAsset != nullptr;
}

void LoadSvoAssetCommand::undo(Database& database)
{
    if (createdAsset)
    {
        database.getProject().removeAsset(createdAsset->getGuid());
    }
}

void LoadSvoAssetCommand::redo(Database& database)
{
    if (createdAsset)
    {
        database.getProject().addAsset(createdAsset);
    }
}

size_t LoadSvoAssetCommand::getUndoSize() const
{
    return sizeof(LoadSvoAssetCommand) + (createdAsset ? createdAsset->getSizeInBytes() : 0);
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("LoadSvoAssetCommand/TypeAndVersion")
{
    LoadSvoAssetCommand cmd;
    cmd.setInput(Polyglot::Node({
      {   "name", "Voxel" },
      { "source", "dummy" }
    }));
    CHECK(cmd.getType() == "LoadSvoAsset");
    CHECK(cmd.getVersion() == 1);
    CHECK_FALSE(cmd.getDescription().empty());
    CHECK(cmd.getRequiredAssets().empty());
}
#endif
