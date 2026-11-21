#include "Commands/Conversion/ConvertSvoToStlSurfaceNetsCommand.h"

#include "CADacomb/Util/ProgressBar.h"
#include "Conversion/AssetConverter.h"
#include "Database/Asset/TriangulationAsset.h"
#include "Database/Asset/VoxelAsset.h"
#include "Database/Database.h"

ConvertSvoToStlSurfaceNetsCommand::ConvertSvoToStlSurfaceNetsCommand() {}

ConvertSvoToStlSurfaceNetsCommand::~ConvertSvoToStlSurfaceNetsCommand() {}

std::string ConvertSvoToStlSurfaceNetsCommand::getType() const
{
    return "ConvertSvoToStlSurfaceNets";
}

int ConvertSvoToStlSurfaceNetsCommand::getVersion() const
{
    return 1;
}

std::string ConvertSvoToStlSurfaceNetsCommand::getDescription() const
{
    return "Command: ConvertSvoToStlSurfaceNets\n"
           "Version: 1\n"
           "Purpose: Converts a volumetric SVO asset to a triangulation STL asset using SurfaceNets.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"ConvertSvoToStlSurfaceNets\",\n"
           "  \"version\": 1,\n"
           "  \"assetGuid\": <string>,\n"
           "  \"targetDepth\": <int>,\n"
           "  \"deleteOrigin\": <bool>\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: None\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"ConvertSvoToStlSurfaceNets\",\n"
           "  \"version\": 1,\n"
           "  \"assetGuid\": <string>\n"
           "}";
}

std::vector<CADacomb::GUID> ConvertSvoToStlSurfaceNetsCommand::getRequiredAssets() const
{
    return { assetGuid };
}

void ConvertSvoToStlSurfaceNetsCommand::setInput(const Polyglot::Node& node)
{
    assetGuid    = node["assetGuid"].asString();
    targetDepth  = node["targetDepth"].asInt();
    deleteOrigin = node["deleteOrigin"].asBool();
}

nlohmann::json ConvertSvoToStlSurfaceNetsCommand::execute(Database& database)
{
    CADacomb::ProgressBar bar(1, "Converting SVO to STL (SurfaceNets)");

    auto baseAsset = database.getProject().getAsset(assetGuid);
    if (!baseAsset || bar.isCanceled())
    {
        return createErrorResponse("AssetNotFound", "Asset not found: " + assetGuid.toString());
    }

    auto voxelAsset = std::dynamic_pointer_cast<VoxelAsset>(baseAsset);
    if (!voxelAsset || bar.isCanceled())
    {
        return createErrorResponse("NotVoxelAsset", "Asset is not a VoxelAsset");
    }

    auto stlAsset = AssetConverter::svoToStlSurfaceNets(*voxelAsset, targetDepth);
    if (!stlAsset || bar.isCanceled())
    {
        return createErrorResponse("ConversionFailed", "Failed to convert SVO asset to STL using SurfaceNets");
    }

    bar.addProgress(1);

    database.getProject().addAsset(stlAsset);
    createdAsset = stlAsset;

    if (deleteOrigin)
    {
        originAsset = baseAsset;
        database.getProject().removeAsset(assetGuid);
    }

    return createResponse({
      { "assetGuid", stlAsset->getGuid().toString() }
    });
}

bool ConvertSvoToStlSurfaceNetsCommand::isUndoable() const
{
    return createdAsset != nullptr;
}

void ConvertSvoToStlSurfaceNetsCommand::undo(Database& database)
{
    if (createdAsset)
    {
        database.getProject().removeAsset(createdAsset->getGuid());
    }
    if (deleteOrigin && originAsset)
    {
        database.getProject().addAsset(originAsset);
    }
}

void ConvertSvoToStlSurfaceNetsCommand::redo(Database& database)
{
    if (deleteOrigin && originAsset)
    {
        database.getProject().removeAsset(originAsset->getGuid());
    }
    if (createdAsset)
    {
        database.getProject().addAsset(createdAsset);
    }
}

size_t ConvertSvoToStlSurfaceNetsCommand::getUndoSize() const
{
    size_t size = sizeof(ConvertSvoToStlSurfaceNetsCommand);
    if (originAsset)
    {
        size += originAsset->getSizeInBytes();
    }
    if (createdAsset)
    {
        size += createdAsset->getSizeInBytes();
    }
    return size;
}

#ifdef ISTESTPROJECT
#include "CADacomb/Volume/SVO/SVO.h"
#include "Polyglot/Node.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("ConvertSvoToStlSurfaceNetsCommand/ConvertsAndUndoes")
{
    Database database;
    auto     voxel = std::make_shared<VoxelAsset>();
    voxel->setName("Box.cadacombsvo");
    auto svo = std::make_unique<CADacomb::SVO<bool>>(8, false);
    for (int z = 2; z < 6; z++)
    {
        for (int y = 2; y < 6; y++)
        {
            for (int x = 2; x < 6; x++)
            {
                svo->setVoxel(glm::ivec3(x, y, z), true);
            }
        }
    }
    voxel->setSVO(std::move(svo));
    voxel->setAABB(CADacomb::AABB3(glm::dvec3(0.0), glm::dvec3(8.0)));
    database.getProject().addAsset(voxel);

    ConvertSvoToStlSurfaceNetsCommand cmd;
    cmd.setInput(Polyglot::Node({
      {    "assetGuid", voxel->getGuid().toString() },
      {  "targetDepth",                          -1 },
      { "deleteOrigin",                       false }
    }));

    nlohmann::json res = cmd.execute(database);
    REQUIRE(res.at("type").get<std::string>() == "ConvertSvoToStlSurfaceNets");
    CADacomb::GUID createdGuid(res.at("assetGuid").get<std::string>());
    auto           created = std::dynamic_pointer_cast<TriangulationAsset>(database.getProject().getAsset(createdGuid));
    REQUIRE(created != nullptr);
    CHECK(created->getTriangulation().vertices.size() > 0);
    CHECK(created->getTriangulation().indices.size() > 0);

    cmd.undo(database);
    CHECK(database.getProject().getAsset(createdGuid) == nullptr);

    cmd.redo(database);
    CHECK(database.getProject().getAsset(createdGuid) != nullptr);
}
#endif
