#include "Commands/Conversion/ConvertSdfToStlCommand.h"

#include "CADacomb/Util/ProgressBar.h"
#include "Conversion/AssetConverter.h"
#include "Database/Asset/SdfAsset.h"
#include "Database/Asset/TriangulationAsset.h"
#include "Database/Database.h"

ConvertSdfToStlCommand::ConvertSdfToStlCommand() {}

ConvertSdfToStlCommand::~ConvertSdfToStlCommand() {}

std::string ConvertSdfToStlCommand::getType() const
{
    return "ConvertSdfToStl";
}

int ConvertSdfToStlCommand::getVersion() const
{
    return 1;
}

std::string ConvertSdfToStlCommand::getDescription() const
{
    return "Command: ConvertSdfToStl\n"
           "Version: 1\n"
           "Purpose: Converts an SdfAsset to a TriangulationAsset using SurfaceNets.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"ConvertSdfToStl\",\n"
           "  \"version\": 1,\n"
           "  \"assetGuid\": <string>,\n"
           "  \"resolution\": <uint>,\n"
           "  \"deleteOrigin\": <bool>\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: None\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"ConvertSdfToStl\",\n"
           "  \"version\": 1,\n"
           "  \"assetGuid\": <string>\n"
           "}";
}

std::vector<CADacomb::GUID> ConvertSdfToStlCommand::getRequiredAssets() const
{
    return { assetGuid };
}

void ConvertSdfToStlCommand::setInput(const Polyglot::Node& node)
{
    assetGuid    = node["assetGuid"].asString();
    resolution   = static_cast<uint32_t>(node["resolution"].asInt());
    deleteOrigin = node["deleteOrigin"].asBool();
}

nlohmann::json ConvertSdfToStlCommand::execute(Database& database)
{
    CADacomb::ProgressBar bar(1, "Converting SDF to STL");

    auto baseAsset = database.getProject().getAsset(assetGuid);
    if (!baseAsset || bar.isCanceled())
    {
        return createErrorResponse("AssetNotFound", "Asset not found: " + assetGuid.toString());
    }

    auto sdfAsset = std::dynamic_pointer_cast<SdfAsset>(baseAsset);
    if (!sdfAsset || bar.isCanceled())
    {
        return createErrorResponse("NotSdfAsset", "Asset is not an SdfAsset");
    }

    auto stlAsset = AssetConverter::sdfToStl(*sdfAsset, resolution);
    if (!stlAsset || bar.isCanceled())
    {
        return createErrorResponse("ConversionFailed", "Failed to convert SDF asset to STL");
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

bool ConvertSdfToStlCommand::isUndoable() const
{
    return createdAsset != nullptr;
}

void ConvertSdfToStlCommand::undo(Database& database)
{
    if (createdAsset)
    {
        database.getProject().removeAsset(createdAsset->getGuid());
    }
    if (originAsset)
    {
        database.getProject().addAsset(originAsset);
    }
}

void ConvertSdfToStlCommand::redo(Database& database)
{
    if (createdAsset)
    {
        database.getProject().addAsset(createdAsset);
    }
    if (originAsset)
    {
        database.getProject().removeAsset(originAsset->getGuid());
    }
}

size_t ConvertSdfToStlCommand::getUndoSize() const
{
    return sizeof(ConvertSdfToStlCommand) + (createdAsset ? createdAsset->getSizeInBytes() : 0) + (originAsset ? originAsset->getSizeInBytes() : 0);
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("ConvertSdfToStlCommand/ConvertAndUndoRedo")
{
    Database database;
    auto     sdfAsset = std::make_shared<SdfAsset>();
    sdfAsset->setName("SphereSdf");
    database.getProject().addAsset(sdfAsset);

    ConvertSdfToStlCommand cmd;
    cmd.setInput({
      {         "type",              "ConvertSdfToStl" },
      {      "version",                              1 },
      {    "assetGuid", sdfAsset->getGuid().toString() },
      {   "resolution",                             16 },
      { "deleteOrigin",                          false }
    });
    nlohmann::json result = cmd.execute(database);
    CHECK(result.at("type").get<std::string>() == "ConvertSdfToStl");

    CADacomb::GUID stlGuid(result.at("assetGuid").get<std::string>());
    REQUIRE(database.getProject().hasAsset(stlGuid));
    CHECK(database.getProject().getAsset(stlGuid)->getType() == "Triangulation");

    cmd.undo(database);
    CHECK_FALSE(database.getProject().hasAsset(stlGuid));

    cmd.redo(database);
    CHECK(database.getProject().hasAsset(stlGuid));
}
#endif
