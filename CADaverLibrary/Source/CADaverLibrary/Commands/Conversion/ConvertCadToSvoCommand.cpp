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

void ConvertCadToSvoCommand::setInput(const Polyglot::Node& node)
{
    assetGuid    = node["assetGuid"].asString();
    precision    = node["precision"].asDouble();
    dimension    = static_cast<uint32_t>(node["dimension"].asInt());
    deleteOrigin = node["deleteOrigin"].asBool();
}

nlohmann::json ConvertCadToSvoCommand::execute(Database& database)
{
    CADacomb::ProgressBar bar(1, "Converting CAD to SVO");

    auto baseAsset = database.getProject().getAsset(assetGuid);
    if (!baseAsset || bar.isCanceled())
    {
        return createErrorResponse("AssetNotFound", "Asset not found: " + assetGuid.toString());
    }

    auto cadAsset = std::dynamic_pointer_cast<CadAsset>(baseAsset);
    if (!cadAsset || bar.isCanceled())
    {
        return createErrorResponse("NotCadAsset", "Asset is not a CadAsset");
    }

    auto svoAsset = AssetConverter::cadToSvo(*cadAsset, precision, dimension);
    if (!svoAsset || bar.isCanceled())
    {
        return createErrorResponse("ConversionFailed", "Failed to convert CAD asset to SVO");
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
