#include "Commands/Conversion/ConvertCadToSvoCommand.h"

#include "CADacomb/Util/ProgressBar.h"
#include "Conversion/AssetConverter.h"
#include "Database/Asset/CadAsset.h"
#include "Database/Asset/VoxelAsset.h"
#include "Database/Database.h"

ConvertCadToSvoCommand::ConvertCadToSvoCommand() {}

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
    createdAsset = svoAsset;

    if (deleteOrigin)
    {
        originAsset = baseAsset;
        database.getProject().removeAsset(assetGuid);
    }

    return createResponse("Success",
                          {
                            { "assetGuid", svoAsset->getGuid().toString() }
    });
}

bool ConvertCadToSvoCommand::isUndoable() const
{
    return createdAsset != nullptr;
}

void ConvertCadToSvoCommand::undo(Database& database)
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

void ConvertCadToSvoCommand::redo(Database& database)
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

size_t ConvertCadToSvoCommand::getUndoSize() const
{
    size_t size = sizeof(ConvertCadToSvoCommand);
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
