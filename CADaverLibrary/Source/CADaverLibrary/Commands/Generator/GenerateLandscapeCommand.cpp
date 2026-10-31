#include "Commands/Generator/GenerateLandscapeCommand.h"

#include "CADacomb/Volume/MergeStrategy/MostPrevalentExceptMergeStrategy.h"
#include "CADacomb/Volume/SVO/Generator/SVOLandscapeGenerator.h"
#include "Database/Asset/VoxelAsset.h"
#include "Database/Database.h"

GenerateLandscapeCommand::GenerateLandscapeCommand() {}

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

void GenerateLandscapeCommand::setInput(const nlohmann::json& json)
{
    dimension              = json.value("dimension", 64u);
    assetName              = json.value("assetName", "Landscape");
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

nlohmann::json GenerateLandscapeCommand::execute(Database& database, std::istream*, std::ostream*)
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
    createdAsset = voxelAsset;

    return createResponse("Success",
                          {
                            { "assetGuid", voxelAsset->getGuid().toString() }
    });
}

bool GenerateLandscapeCommand::isUndoable() const
{
    return createdAsset != nullptr;
}

void GenerateLandscapeCommand::undo(Database& database)
{
    if (createdAsset)
    {
        database.getProject().removeAsset(createdAsset->getGuid());
    }
}

void GenerateLandscapeCommand::redo(Database& database)
{
    if (createdAsset)
    {
        database.getProject().addAsset(createdAsset);
    }
}

size_t GenerateLandscapeCommand::getUndoSize() const
{
    return sizeof(GenerateLandscapeCommand) + (createdAsset ? createdAsset->getSizeInBytes() : 0);
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
    Database                 database;
    GenerateLandscapeCommand cmd;
    cmd.setInput({
      {      "seed",            42 },
      { "dimension",            16 },
      { "assetName", "TestTerrain" }
    });
    nlohmann::json result = cmd.execute(database);

    CHECK(result.at("status").get<std::string>() == "Success");
    std::string guidStr = result.at("assetGuid").get<std::string>();
    CHECK_FALSE(guidStr.empty());

    CADacomb::GUID guid(guidStr);
    auto           asset = database.getProject().getAsset(guid);
    REQUIRE(asset != nullptr);
    CHECK(asset->getName() == "TestTerrain");
    CHECK(cmd.isUndoable());
}

TEST_CASE("GenerateLandscapeCommand/UndoRedo")
{
    Database                 database;
    GenerateLandscapeCommand cmd;
    cmd.setInput({
      { "dimension",            16 },
      { "assetName", "UndoTerrain" }
    });
    nlohmann::json result  = cmd.execute(database);
    std::string    guidStr = result.at("assetGuid").get<std::string>();
    CADacomb::GUID guid(guidStr);

    REQUIRE(database.getProject().hasAsset(guid));

    cmd.undo(database);
    CHECK_FALSE(database.getProject().hasAsset(guid));

    cmd.redo(database);
    CHECK(database.getProject().hasAsset(guid));
}
#endif
