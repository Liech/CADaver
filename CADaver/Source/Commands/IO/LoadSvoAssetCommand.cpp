#include "Commands/IO/LoadSvoAssetCommand.h"

#include "CADacomb/Util/ProgressBar.h"
#include "CADacomb/Volume/SVO/SVOFile.h"
#include "Commands/Undo/AddAssetUndoRecord.h"
#include "Commands/Undo/UndoStack.h"
#include "Database/Asset/VoxelAsset.h"
#include "Database/Database.h"

LoadSvoAssetCommand::LoadSvoAssetCommand() {}

LoadSvoAssetCommand::LoadSvoAssetCommand(const std::string& name)
  : name(name)
{
}

LoadSvoAssetCommand::LoadSvoAssetCommand(const nlohmann::json& json)
  : name(json.at("name").get<std::string>())
{
}

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
           "Purpose: Loads an SVO voxel asset from an input stream into the project.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"LoadSvoAsset\",\n"
           "  \"version\": 1,\n"
           "  \"name\": \"<string>\"\n"
           "}\n\n"
           "Input Stream: SVO binary stream\n"
           "Output Stream: None\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"LoadSvoAsset\",\n"
           "  \"version\": 1,\n"
           "  \"status\": \"Success\" | \"NoInputStream\" | \"ParseFailed\",\n"
           "  \"index\": <int>,\n"
           "  \"name\": \"<string>\"\n"
           "}";
}

nlohmann::json LoadSvoAssetCommand::execute(Database& database, std::istream* inputStream, std::ostream*, bool recordUndo)
{
    CADacomb::ProgressBar bar(1, "Loading SVO asset");

    if (!inputStream || bar.isCanceled())
    {
        return createResponse("NoInputStream",
                              {
                                { "index",   -1 },
                                {  "name", name }
        });
    }

    CADacomb::AABB3                      bounds;
    std::unique_ptr<CADacomb::SVO<bool>> svo = CADacomb::SVOFile::load<bool>(*inputStream, bounds);
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

    if (recordUndo)
    {
        UndoStack::getInstance().push(std::make_unique<AddAssetUndoRecord>(asset));
    }

    return createResponse("Success",
                          {
                            { "assetGuid", asset->getGuid().toString() },
                            {      "name",                        name }
    });
}

const std::string& LoadSvoAssetCommand::getName() const
{
    return name;
}

void LoadSvoAssetCommand::setName(const std::string& newName)
{
    name = newName;
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("LoadSvoAssetCommand/TypeAndVersion")
{
    LoadSvoAssetCommand cmd(std::string("Voxel"));
    CHECK(cmd.getType() == "LoadSvoAsset");
    CHECK(cmd.getVersion() == 1);
    CHECK_FALSE(cmd.getDescription().empty());
    CHECK(cmd.getName() == "Voxel");
    CHECK(cmd.getRequiredAssets().empty());
}
#endif
