#include "Commands/Converter/ConvertCadToSvoCommand.h"

#include "CADacomb/Util/ProgressBar.h"
#include "Conversion/AssetConverter.h"
#include "Database/Asset/CadAsset.h"
#include "Database/Asset/VoxelAsset.h"
#include "Database/Database.h"
#include "Undo/AddAssetUndoRecord.h"
#include "Undo/ReplaceAssetUndoRecord.h"
#include "Undo/UndoStack.h"

ConvertCadToSvoCommand::ConvertCadToSvoCommand() {}

ConvertCadToSvoCommand::ConvertCadToSvoCommand(const CADacomb::GUID& assetGuid, double precision, uint32_t dimension, bool deleteOrigin)
  : assetGuid(assetGuid)
  , precision(precision)
  , dimension(dimension)
  , deleteOrigin(deleteOrigin)
{
}

ConvertCadToSvoCommand::ConvertCadToSvoCommand(const nlohmann::json& json)
  : assetGuid(json.value("assetGuid", ""))
  , precision(json.value("precision", 0.1))
  , dimension(json.value("dimension", 64u))
  , deleteOrigin(json.value("deleteOrigin", false))
{
}

ConvertCadToSvoCommand::~ConvertCadToSvoCommand() {}

std::string ConvertCadToSvoCommand::getType() const
{
    return "ConvertCadToSvo";
}

int ConvertCadToSvoCommand::getVersion() const
{
    return 1;
}

std::string ConvertCadToSvoCommand::getDescription() const
{
    return "Command: ConvertCadToSvo\n"
           "Version: 1\n"
           "Purpose: Converts a CAD asset to a volumetric SVO asset via triangulation.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"ConvertCadToSvo\",\n"
           "  \"version\": 1,\n"
           "  \"assetGuid\": <string>,\n"
           "  \"precision\": <double> (optional, default: 0.1),\n"
           "  \"dimension\": <uint> (optional, default: 64),\n"
           "  \"deleteOrigin\": <bool> (optional, default: false)\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: None\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"ConvertCadToSvo\",\n"
           "  \"version\": 1,\n"
           "  \"status\": \"Success\" | \"AssetNotFound\" | \"NotCadAsset\" | \"ConversionFailed\",\n"
           "  \"assetGuid\": <string>\n"
           "}";
}

std::vector<CADacomb::GUID> ConvertCadToSvoCommand::getRequiredAssets() const
{
    return { assetGuid };
}

void ConvertCadToSvoCommand::setInput(const nlohmann::json& json)
{
    assetGuid    = json.value("assetGuid", "");
    precision    = json.value("precision", 0.1);
    dimension    = json.value("dimension", 64u);
    deleteOrigin = json.value("deleteOrigin", false);
}

nlohmann::json ConvertCadToSvoCommand::execute(Database& database, std::istream*, std::ostream*)
{
    CADacomb::ProgressBar bar(1, "Converting CAD to SVO");

    auto baseAsset = database.getProject().getAsset(assetGuid);
    if (!baseAsset || bar.isCanceled())
    {
        return createResponse("AssetNotFound",
                              {
                                { "assetGuid", "" }
        });
    }

    auto cadAsset = std::dynamic_pointer_cast<CadAsset>(baseAsset);
    if (!cadAsset || bar.isCanceled())
    {
        return createResponse("NotCadAsset",
                              {
                                { "assetGuid", "" }
        });
    }

    auto svoAsset = AssetConverter::cadToSvo(*cadAsset, precision, dimension);
    if (!svoAsset || bar.isCanceled())
    {
        return createResponse("ConversionFailed",
                              {
                                { "assetGuid", "" }
        });
    }

    bar.addProgress(1);

    database.getProject().addAsset(svoAsset);

    if (deleteOrigin)
    {
        database.getProject().removeAsset(assetGuid);

        UndoStack::getInstance().push(std::make_unique<ReplaceAssetUndoRecord>(baseAsset, svoAsset));
    }
    else
    {

        UndoStack::getInstance().push(std::make_unique<AddAssetUndoRecord>(svoAsset));
    }

    return createResponse("Success",
                          {
                            { "assetGuid", svoAsset->getGuid().toString() }
    });
}

const CADacomb::GUID& ConvertCadToSvoCommand::getAssetGuid() const
{
    return assetGuid;
}

void ConvertCadToSvoCommand::setAssetGuid(const CADacomb::GUID& newGuid)
{
    assetGuid = newGuid;
}

double ConvertCadToSvoCommand::getPrecision() const
{
    return precision;
}

void ConvertCadToSvoCommand::setPrecision(double newPrecision)
{
    precision = newPrecision;
}

uint32_t ConvertCadToSvoCommand::getDimension() const
{
    return dimension;
}

void ConvertCadToSvoCommand::setDimension(uint32_t newDimension)
{
    dimension = newDimension;
}

bool ConvertCadToSvoCommand::getDeleteOrigin() const
{
    return deleteOrigin;
}

void ConvertCadToSvoCommand::setDeleteOrigin(bool shouldDelete)
{
    deleteOrigin = shouldDelete;
}
