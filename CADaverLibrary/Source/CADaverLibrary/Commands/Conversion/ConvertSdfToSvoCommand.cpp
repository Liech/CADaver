#include "Commands/Conversion/ConvertSdfToSvoCommand.h"

#include "CADacomb/Util/ProgressBar.h"
#include "Conversion/AssetConverter.h"
#include "Database/Asset/SdfAsset.h"
#include "Database/Asset/VoxelAsset.h"
#include "Database/Database.h"

ConvertSdfToSvoCommand::ConvertSdfToSvoCommand() {}

ConvertSdfToSvoCommand::~ConvertSdfToSvoCommand() {}

std::string ConvertSdfToSvoCommand::getType() const
{
    return "ConvertSdfToSvo";
}

int ConvertSdfToSvoCommand::getVersion() const
{
    return 1;
}

std::string ConvertSdfToSvoCommand::getDescription() const
{
    return "Command: ConvertSdfToSvo\n"
           "Version: 1\n"
           "Purpose: Converts an SdfAsset to a volumetric VoxelAsset (SVO).\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"ConvertSdfToSvo\",\n"
           "  \"version\": 1,\n"
           "  \"assetGuid\": <string>,\n"
           "  \"dimension\": <uint>,\n"
           "  \"deleteOrigin\": <bool>\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: None\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"ConvertSdfToSvo\",\n"
           "  \"version\": 1,\n"
           "  \"assetGuid\": <string>\n"
           "}";
}

std::vector<CADacomb::GUID> ConvertSdfToSvoCommand::getRequiredAssets() const
{
    return { assetGuid };
}

void ConvertSdfToSvoCommand::setInput(const Polyglot::Node& node)
{
    assetGuid    = node["assetGuid"].asString();
    dimension    = static_cast<uint32_t>(node["dimension"].asInt());
    deleteOrigin = node["deleteOrigin"].asBool();
}

nlohmann::json ConvertSdfToSvoCommand::execute(Database& database)
{
    CADacomb::ProgressBar bar(1, "Converting SDF to SVO");

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

    auto svoAsset = AssetConverter::sdfToSvo(*sdfAsset, dimension);
    if (!svoAsset || bar.isCanceled())
    {
        return createErrorResponse("ConversionFailed", "Failed to convert SDF asset to SVO");
    }

    bar.addProgress(1);

    database.getProject().addAsset(svoAsset);
    createdAsset = svoAsset;

    if (deleteOrigin)
    {
        originAsset = baseAsset;
        database.getProject().removeAsset(assetGuid);
    }

    return createResponse({
      { "assetGuid", svoAsset->getGuid().toString() }
    });
}

bool ConvertSdfToSvoCommand::isUndoable() const
{
    return createdAsset != nullptr;
}

void ConvertSdfToSvoCommand::undo(Database& database)
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

void ConvertSdfToSvoCommand::redo(Database& database)
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

size_t ConvertSdfToSvoCommand::getUndoSize() const
{
    return sizeof(ConvertSdfToSvoCommand) + (createdAsset ? createdAsset->getSizeInBytes() : 0) + (originAsset ? originAsset->getSizeInBytes() : 0);
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("ConvertSdfToSvoCommand/ConvertAndUndoRedo")
{
    Database database;
    auto     sdfAsset = std::make_shared<SdfAsset>();
    sdfAsset->setName("SphereSdf");
    database.getProject().addAsset(sdfAsset);

    ConvertSdfToSvoCommand cmd;
    cmd.setInput({
      {         "type",              "ConvertSdfToSvo" },
      {      "version",                              1 },
      {    "assetGuid", sdfAsset->getGuid().toString() },
      {    "dimension",                             16 },
      { "deleteOrigin",                          false }
    });
    nlohmann::json result = cmd.execute(database);
    CHECK(result.at("type").get<std::string>() == "ConvertSdfToSvo");

    CADacomb::GUID svoGuid(result.at("assetGuid").get<std::string>());
    REQUIRE(database.getProject().hasAsset(svoGuid));
    CHECK(database.getProject().getAsset(svoGuid)->getType() == "Voxel");

    cmd.undo(database);
    CHECK_FALSE(database.getProject().hasAsset(svoGuid));

    cmd.redo(database);
    CHECK(database.getProject().hasAsset(svoGuid));
}
#endif
