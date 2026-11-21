#include "Commands/Conversion/ConvertSvoToStlMarchingCubesCommand.h"

#include "CADacomb/Util/ProgressBar.h"
#include "Conversion/AssetConverter.h"
#include "Database/Asset/TriangulationAsset.h"
#include "Database/Asset/VoxelAsset.h"
#include "Database/Database.h"

ConvertSvoToStlMarchingCubesCommand::ConvertSvoToStlMarchingCubesCommand() {}

ConvertSvoToStlMarchingCubesCommand::~ConvertSvoToStlMarchingCubesCommand() {}

std::string ConvertSvoToStlMarchingCubesCommand::getType() const
{
    return "ConvertSvoToStlMarchingCubes";
}

int ConvertSvoToStlMarchingCubesCommand::getVersion() const
{
    return 1;
}

std::string ConvertSvoToStlMarchingCubesCommand::getDescription() const
{
    return "Command: ConvertSvoToStlMarchingCubes\n"
           "Version: 1\n"
           "Purpose: Converts a volumetric SVO asset to a triangulation STL asset using Marching Cubes.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"ConvertSvoToStlMarchingCubes\",\n"
           "  \"version\": 1,\n"
           "  \"assetGuid\": <string>,\n"
           "  \"targetDepth\": <int>,\n"
           "  \"deleteOrigin\": <bool>\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: None\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"ConvertSvoToStlMarchingCubes\",\n"
           "  \"version\": 1,\n"
           "  \"assetGuid\": <string>\n"
           "}";
}

std::vector<CADacomb::GUID> ConvertSvoToStlMarchingCubesCommand::getRequiredAssets() const
{
    return { assetGuid };
}

void ConvertSvoToStlMarchingCubesCommand::setInput(const Polyglot::Node& node)
{
    assetGuid    = node["assetGuid"].asString();
    targetDepth  = node["targetDepth"].asInt();
    deleteOrigin = node["deleteOrigin"].asBool();
}

nlohmann::json ConvertSvoToStlMarchingCubesCommand::execute(Database& database)
{
    CADacomb::ProgressBar bar(1, "Converting SVO to STL (Marching Cubes)");

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

    auto stlAsset = AssetConverter::svoToStlMarchingCubes(*voxelAsset, targetDepth);
    if (!stlAsset || bar.isCanceled())
    {
        return createErrorResponse("ConversionFailed", "Failed to convert SVO asset to STL using Marching Cubes");
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

bool ConvertSvoToStlMarchingCubesCommand::isUndoable() const
{
    return createdAsset != nullptr;
}

void ConvertSvoToStlMarchingCubesCommand::undo(Database& database)
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

void ConvertSvoToStlMarchingCubesCommand::redo(Database& database)
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

size_t ConvertSvoToStlMarchingCubesCommand::getUndoSize() const
{
    size_t size = sizeof(ConvertSvoToStlMarchingCubesCommand);
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

TEST_CASE("ConvertSvoToStlMarchingCubesCommand/ConvertsAndUndoes")
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

    ConvertSvoToStlMarchingCubesCommand cmd;
    cmd.setInput(Polyglot::Node({
      {    "assetGuid", voxel->getGuid().toString() },
      {  "targetDepth",                          -1 },
      { "deleteOrigin",                       false }
    }));

    nlohmann::json res = cmd.execute(database);
    REQUIRE(res.at("type").get<std::string>() == "ConvertSvoToStlMarchingCubes");
    CADacomb::GUID createdGuid(res.at("assetGuid").get<std::string>());
    auto           created = std::dynamic_pointer_cast<TriangulationAsset>(database.getProject().getAsset(createdGuid));
    REQUIRE(created != nullptr);
    const auto& tri = created->getTriangulation();
    CHECK(tri.vertices.size() > 0);
    CHECK(tri.indices.size() > 0);

    glm::dvec3 center(4.0, 4.0, 4.0);
    size_t     outwardCount = 0;
    size_t     faceCount    = tri.indices.size() / 3;
    for (size_t f = 0; f < faceCount; f++)
    {
        const glm::dvec3& v0       = tri.vertices[tri.indices[f * 3 + 0]];
        const glm::dvec3& v1       = tri.vertices[tri.indices[f * 3 + 1]];
        const glm::dvec3& v2       = tri.vertices[tri.indices[f * 3 + 2]];
        glm::dvec3        centroid = (v0 + v1 + v2) / 3.0;
        glm::dvec3        normal   = glm::cross(v1 - v0, v2 - v0);
        if (glm::dot(normal, centroid - center) > 0.0)
        {
            outwardCount++;
        }
    }
    CHECK(outwardCount == faceCount);

    cmd.undo(database);
    CHECK(database.getProject().getAsset(createdGuid) == nullptr);

    cmd.redo(database);
    CHECK(database.getProject().getAsset(createdGuid) != nullptr);
}
#endif
