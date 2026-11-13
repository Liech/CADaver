#include "Commands/Geometry/GetProjectAABBCommand.h"

#include "CADacomb/CAD/Interface/Shape.h"
#include "Database/Asset/CadAsset.h"
#include "Database/Asset/TriangulationAsset.h"
#include "Database/Asset/VoxelAsset.h"
#include "Database/Database.h"

#include <algorithm>
#include <limits>

GetProjectAABBCommand::GetProjectAABBCommand() {}

GetProjectAABBCommand::~GetProjectAABBCommand() {}

std::string GetProjectAABBCommand::getType() const
{
    return "GetProjectAABB";
}

int GetProjectAABBCommand::getVersion() const
{
    return 1;
}

std::string GetProjectAABBCommand::getDescription() const
{
    return "Command: GetProjectAABB\n"
           "Version: 1\n"
           "Purpose: Queries the combined axis-aligned bounding box across all geometry assets in the project.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"GetProjectAABB\",\n"
           "  \"version\": 1\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: None\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"GetProjectAABB\",\n"
           "  \"version\": 1,\n"
           "  \"status\": \"Success\" | \"EmptyProject\",\n"
           "  \"min\": [<double>, <double>, <double>],\n"
           "  \"max\": [<double>, <double>, <double>]\n"
           "}";
}

nlohmann::json GetProjectAABBCommand::execute(Database& database)
{
    const auto& assets = database.getProject().getAssets();
    glm::dvec3  overallMin(std::numeric_limits<double>::max());
    glm::dvec3  overallMax(std::numeric_limits<double>::lowest());
    bool        foundValidBounds = false;

    for (const auto& asset : assets)
    {
        if (!asset)
        {
            continue;
        }

        auto voxelAsset = std::dynamic_pointer_cast<const VoxelAsset>(asset);
        if (voxelAsset)
        {
            const auto& aabb = voxelAsset->getAABB();
            overallMin       = glm::min(overallMin, glm::dvec3(aabb.getMin()));
            overallMax       = glm::max(overallMax, glm::dvec3(aabb.getMax()));
            foundValidBounds = true;
            continue;
        }

        auto triAsset = std::dynamic_pointer_cast<const TriangulationAsset>(asset);
        if (triAsset && !triAsset->getTriangulation().vertices.empty())
        {
            auto [pos, size] = triAsset->getTriangulation().getAABB();
            overallMin       = glm::min(overallMin, pos);
            overallMax       = glm::max(overallMax, pos + size);
            foundValidBounds = true;
            continue;
        }

        auto cadAsset = std::dynamic_pointer_cast<const CadAsset>(asset);
        if (cadAsset)
        {
            if (!cadAsset->getTriangulation().vertices.empty())
            {
                auto [pos, size] = cadAsset->getTriangulation().getAABB();
                overallMin       = glm::min(overallMin, pos);
                overallMax       = glm::max(overallMax, pos + size);
                foundValidBounds = true;
            }
            else
            {
                try
                {
                    auto aabb        = cadAsset->getCadShape().getAABB();
                    overallMin       = glm::min(overallMin, glm::dvec3(aabb.getMin()));
                    overallMax       = glm::max(overallMax, glm::dvec3(aabb.getMax()));
                    foundValidBounds = true;
                }
                catch (...)
                {
                }
            }
        }
    }

    if (!foundValidBounds)
    {
        return createResponse("EmptyProject",
                              {
                                { "min", { 0.0, 0.0, 0.0 } },
                                { "max", { 0.0, 0.0, 0.0 } }
        });
    }

    return createResponse("Success",
                          {
                            { "min", { overallMin.x, overallMin.y, overallMin.z } },
                            { "max", { overallMax.x, overallMax.y, overallMax.z } }
    });
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("GetProjectAABBCommand/EmptyProject")
{
    Database              database;
    GetProjectAABBCommand cmd;

    nlohmann::json result = cmd.execute(database);
    CHECK(result.at("status") == "EmptyProject");
}

TEST_CASE("GetProjectAABBCommand/CombinedBounds")
{
    Database database;
    auto     a1 = std::make_shared<VoxelAsset>();
    a1->setAABB(CADacomb::AABB3(glm::vec3(-2.0f, -3.0f, -4.0f), glm::vec3(1.0f, 1.0f, 1.0f)));
    auto a2 = std::make_shared<VoxelAsset>();
    a2->setAABB(CADacomb::AABB3(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(5.0f, 6.0f, 7.0f)));
    database.getProject().addAsset(a1);
    database.getProject().addAsset(a2);

    GetProjectAABBCommand cmd;
    nlohmann::json        result = cmd.execute(database);
    CHECK(result.at("status") == "Success");
    CHECK(result.at("min")[0] == -2.0);
    CHECK(result.at("min")[1] == -3.0);
    CHECK(result.at("min")[2] == -4.0);
    CHECK(result.at("max")[0] == 5.0);
    CHECK(result.at("max")[1] == 6.0);
    CHECK(result.at("max")[2] == 7.0);
}
#endif
