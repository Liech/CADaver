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
           "  \"assetName\": <string>,\n"
           "  \"dimension\": <uint>,\n"
           "  \"seed\": <uint>,\n"
           "  \"octaves\": <int>,\n"
           "  \"frequency\": <double>,\n"
           "  \"persistence\": <double>,\n"
           "  \"lacunarity\": <double>,\n"
           "  \"baseHeightRatio\": <double>,\n"
           "  \"heightScale\": <double>,\n"
           "  \"enableCaves\": <bool>,\n"
           "  \"caveFrequency\": <double>,\n"
           "  \"caveThreshold\": <double>,\n"
           "  \"solidThreshold\": <double>\n"
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

void GenerateLandscapeCommand::setInput(const Polyglot::Node& node)
{
    dimension              = static_cast<uint32_t>(node["dimension"].asInt());
    assetName              = node["assetName"].asString();
    config.seed            = static_cast<uint32_t>(node["seed"].asInt());
    config.octaves         = node["octaves"].asInt();
    config.frequency       = node["frequency"].asDouble();
    config.persistence     = node["persistence"].asDouble();
    config.lacunarity      = node["lacunarity"].asDouble();
    config.baseHeightRatio = node["baseHeightRatio"].asDouble();
    config.heightScale     = node["heightScale"].asDouble();
    config.enableCaves     = node["enableCaves"].asBool();
    config.caveFrequency   = node["caveFrequency"].asDouble();
    config.caveThreshold   = node["caveThreshold"].asDouble();
    config.solidThreshold  = node["solidThreshold"].asDouble();
}

nlohmann::json GenerateLandscapeCommand::execute(Database& database)
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
      {            "type", "GenerateLandscape" },
      {         "version",                   1 },
      {       "assetName",       "TestTerrain" },
      {       "dimension",                  16 },
      {            "seed",                  42 },
      {         "octaves",                   4 },
      {       "frequency",                0.05 },
      {     "persistence",                 0.5 },
      {      "lacunarity",                 2.0 },
      { "baseHeightRatio",                 0.4 },
      {     "heightScale",                 1.2 },
      {     "enableCaves",               false },
      {   "caveFrequency",                0.04 },
      {   "caveThreshold",                0.12 },
      {  "solidThreshold",                 0.0 }
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
      {            "type", "GenerateLandscape" },
      {         "version",                   1 },
      {       "assetName",       "UndoTerrain" },
      {       "dimension",                  16 },
      {            "seed",                  42 },
      {         "octaves",                   4 },
      {       "frequency",                0.05 },
      {     "persistence",                 0.5 },
      {      "lacunarity",                 2.0 },
      { "baseHeightRatio",                 0.4 },
      {     "heightScale",                 1.2 },
      {     "enableCaves",               false },
      {   "caveFrequency",                0.04 },
      {   "caveThreshold",                0.12 },
      {  "solidThreshold",                 0.0 }
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
