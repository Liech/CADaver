#include "Commands/Geometry/GetProjectAABBCommand.h"

#include "CADacomb/Util/AABB3.h"
#include "Database/Database.h"
#include "Picking/AssetRayPicker/AssetRayPicker.h"
#include "Picking/AssetRayPicker/AssetRayPickerFactory.h"

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
           "  \"min\": [<double>, <double>, <double>],\n"
           "  \"max\": [<double>, <double>, <double>]\n"
           "}";
}

nlohmann::json GetProjectAABBCommand::execute(Database& database)
{
    const auto&           assets = database.getProject().getAssets();
    CADacomb::AABB3       overallBounds;
    bool                  foundValidBounds = false;
    AssetRayPickerFactory pickerFactory;

    for (const auto& asset : assets)
    {
        if (!asset)
        {
            continue;
        }

        auto picker = pickerFactory.create(asset->getType());
        if (!picker)
        {
            continue;
        }

        CADacomb::AABB3 localBounds = picker->getAABB(*asset);
        if (localBounds.getMin().x <= localBounds.getMax().x)
        {
            CADacomb::AABB3 worldBounds = localBounds.transformed(asset->getTransformation());
            if (!foundValidBounds)
            {
                overallBounds    = worldBounds;
                foundValidBounds = true;
            }
            else
            {
                overallBounds.expand(worldBounds);
            }
        }
    }

    if (!foundValidBounds)
    {
        return createErrorResponse("EmptyProject", "Project contains no assets with valid bounds");
    }

    return createResponse({
      { "min", { overallBounds.getMin().x, overallBounds.getMin().y, overallBounds.getMin().z } },
      { "max", { overallBounds.getMax().x, overallBounds.getMax().y, overallBounds.getMax().z } }
    });
}

#ifdef ISTESTPROJECT
#include "Database/Asset/VoxelAsset.h"

#include <catch2/catch_test_macros.hpp>
#include <glm/gtc/matrix_transform.hpp>

TEST_CASE("GetProjectAABBCommand/EmptyProject")
{
    Database              database;
    GetProjectAABBCommand cmd;

    nlohmann::json result = cmd.execute(database);
    CHECK(result.at("type") == "Error");
    CHECK(result.at("command") == "GetProjectAABB");
    CHECK(result.at("errorType") == "EmptyProject");
}

TEST_CASE("GetProjectAABBCommand/CombinedBounds")
{
    Database database;
    auto     a1 = std::make_shared<VoxelAsset>();
    a1->setAABB(CADacomb::AABB3(glm::vec3(-2.0f, -3.0f, -4.0f), glm::vec3(1.0f, 1.0f, 1.0f)));
    auto a2 = std::make_shared<VoxelAsset>();
    a2->setAABB(CADacomb::AABB3(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(5.0f, 6.0f, 7.0f)));
    a2->setTransformation(glm::translate(glm::dmat4(1.0), glm::dvec3(2.0, 0.0, 0.0)));
    database.getProject().addAsset(a1);
    database.getProject().addAsset(a2);

    GetProjectAABBCommand cmd;
    nlohmann::json        result = cmd.execute(database);
    CHECK(result.at("type") == "GetProjectAABB");
    CHECK(result.at("min")[0] == -2.0);
    CHECK(result.at("min")[1] == -3.0);
    CHECK(result.at("min")[2] == -4.0);
    CHECK(result.at("max")[0] == 7.0);
    CHECK(result.at("max")[1] == 6.0);
    CHECK(result.at("max")[2] == 7.0);
}
#endif
