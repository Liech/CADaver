#include "Commands/Generator/GenerateLandscapeCommand.h"

#include "CADacomb/Volume/MergeStrategy/MostPrevalentExceptMergeStrategy.h"
#include "CADacomb/Volume/SVO/Generator/SVOLandscapeGenerator.h"
#include "Commands/Undo/AddAssetUndoRecord.h"
#include "Database/Asset/VoxelAsset.h"
#include "Database/Database.h"

GenerateLandscapeCommand::GenerateLandscapeCommand() {}

GenerateLandscapeCommand::GenerateLandscapeCommand(const CADacomb::LandscapeConfig& config, uint32_t dimension, const std::string& assetName)
  : config(config)
  , dimension(dimension)
  , assetName(assetName)
{
}

GenerateLandscapeCommand::GenerateLandscapeCommand(const nlohmann::json& json)
  : dimension(json.value("dimension", 64u))
  , assetName(json.value("assetName", "Landscape"))
{
    config.seed            = json.value("seed", 1337u);
    config.octaves         = json.value("octaves", 4);
    config.frequency       = json.value("frequency", 0.05);
    config.persistence     = json.value("persistence", 0.5);
    config.lacunarity      = json.value("lacunarity", 2.0);
    config.baseHeightRatio = json.value("baseHeightRatio", 0.4);
    config.heightScale     = json.value("heightScale", 1.2);
    config.enableCaves     = json.value("enableCaves", false);
    config.caveFrequency   = json.value("caveFrequency", 0.04);
    config.caveThreshold   = json.value("caveThreshold", 0.12);
    config.solidThreshold  = json.value("solidThreshold", 0.0);
}

GenerateLandscapeCommand::~GenerateLandscapeCommand() {}

std::string GenerateLandscapeCommand::getType() const
{
    return "GenerateLandscape";
}

int GenerateLandscapeCommand::getVersion() const
{
    return 1;
}

std::string GenerateLandscapeCommand::getDescription() const
{
    return "Command: GenerateLandscape\n"
           "Version: 1\n"
           "Purpose: Generates a volumetric landscape SVO asset.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"GenerateLandscape\",\n"
           "  \"version\": 1,\n"
           "  \"assetName\": <string> (optional, default: \"Landscape\"),\n"
           "  \"dimension\": <uint> (optional, default: 64),\n"
           "  \"seed\": <uint> (optional, default: 1337),\n"
           "  \"octaves\": <int> (optional, default: 4),\n"
           "  \"frequency\": <double> (optional, default: 0.05),\n"
           "  \"persistence\": <double> (optional, default: 0.5),\n"
           "  \"lacunarity\": <double> (optional, default: 2.0),\n"
           "  \"baseHeightRatio\": <double> (optional, default: 0.4),\n"
           "  \"heightScale\": <double> (optional, default: 1.2),\n"
           "  \"enableCaves\": <bool> (optional, default: false),\n"
           "  \"caveFrequency\": <double> (optional, default: 0.04),\n"
           "  \"caveThreshold\": <double> (optional, default: 0.12),\n"
           "  \"solidThreshold\": <double> (optional, default: 0.0)\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: None\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"GenerateLandscape\",\n"
           "  \"version\": 1,\n"
           "  \"status\": \"Success\" | \"GenerationFailed\",\n"
           "  \"assetGuid\": <string>\n"
           "}";
}

nlohmann::json GenerateLandscapeCommand::execute(Database& database, std::istream*, std::ostream*, bool recordUndo)
{
    auto svo = CADacomb::SVOLandscapeGenerator::generateSVO<bool>(dimension, config, glm::dvec3(0.0), true, false, 32, CADacomb::MostPrevalentExceptMergeStrategy<bool, false>::instance());

    if (!svo)
    {
        return createResponse("GenerationFailed",
                              {
                                { "assetGuid", "" }
        });
    }

    auto voxelAsset = std::make_shared<VoxelAsset>();
    voxelAsset->setName(assetName);
    voxelAsset->setSVO(std::move(svo));
    voxelAsset->setAABB(CADacomb::AABB3(glm::dvec3(0.0), glm::dvec3(static_cast<double>(dimension))));

    database.getProject().addAsset(voxelAsset);
    database.getProject().setSelectedAssetGuid(voxelAsset->getGuid());

    if (recordUndo)
    {
        database.getUndoStack().push(std::make_unique<AddAssetUndoRecord>(voxelAsset));
    }

    return createResponse("Success",
                          {
                            { "assetGuid", voxelAsset->getGuid().toString() }
    });
}

const CADacomb::LandscapeConfig& GenerateLandscapeCommand::getConfig() const
{
    return config;
}

void GenerateLandscapeCommand::setConfig(const CADacomb::LandscapeConfig& newConfig)
{
    config = newConfig;
}

uint32_t GenerateLandscapeCommand::getDimension() const
{
    return dimension;
}

void GenerateLandscapeCommand::setDimension(uint32_t newDimension)
{
    dimension = newDimension;
}

const std::string& GenerateLandscapeCommand::getAssetName() const
{
    return assetName;
}

void GenerateLandscapeCommand::setAssetName(const std::string& newName)
{
    assetName = newName;
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("GenerateLandscapeCommand/TypeAndVersion")
{
    GenerateLandscapeCommand cmd;
    CHECK(cmd.getType() == "GenerateLandscape");
    CHECK(cmd.getVersion() == 1);
    CHECK_FALSE(cmd.getDescription().empty());
}

TEST_CASE("GenerateLandscapeCommand/ExecuteSuccess")
{
    Database                  database;
    CADacomb::LandscapeConfig config;
    config.seed = 42;

    GenerateLandscapeCommand cmd(config, 16, "TestTerrain");
    nlohmann::json           result = cmd.execute(database, nullptr, nullptr, true);

    CHECK(result.at("status").get<std::string>() == "Success");
    std::string guidStr = result.at("assetGuid").get<std::string>();
    CHECK_FALSE(guidStr.empty());

    CADacomb::GUID guid(guidStr);
    auto           asset = database.getProject().getAsset(guid);
    REQUIRE(asset != nullptr);
    CHECK(asset->getName() == "TestTerrain");
    CHECK(database.getProject().getSelectedAssetGuid() == guid);
    CHECK(database.getUndoStack().canUndo());
}

TEST_CASE("GenerateLandscapeCommand/UndoRedo")
{
    Database                  database;
    CADacomb::LandscapeConfig config;

    GenerateLandscapeCommand cmd(config, 16, "UndoTerrain");
    nlohmann::json           result  = cmd.execute(database, nullptr, nullptr, true);
    std::string              guidStr = result.at("assetGuid").get<std::string>();
    CADacomb::GUID           guid(guidStr);

    REQUIRE(database.getProject().hasAsset(guid));

    database.getUndoStack().undo(database);
    CHECK_FALSE(database.getProject().hasAsset(guid));

    database.getUndoStack().redo(database);
    CHECK(database.getProject().hasAsset(guid));
}
#endif
