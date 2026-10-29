#include "Commands/Converter/ConvertStlToSvoCommand.h"

#include "Commands/Undo/AddAssetUndoRecord.h"
#include "Commands/Undo/ReplaceAssetUndoRecord.h"
#include "Database/Asset/TriangulationAsset.h"
#include "Database/Asset/VoxelAsset.h"
#include "Database/AssetConverter.h"
#include "Database/Database.h"

ConvertStlToSvoCommand::ConvertStlToSvoCommand() {}

ConvertStlToSvoCommand::ConvertStlToSvoCommand(const CADacomb::GUID& assetGuid, uint32_t dimension, bool deleteOrigin)
  : assetGuid(assetGuid)
  , dimension(dimension)
  , deleteOrigin(deleteOrigin)
{
}

ConvertStlToSvoCommand::ConvertStlToSvoCommand(const nlohmann::json& json)
  : assetGuid(json.value("assetGuid", ""))
  , dimension(json.value("dimension", 64u))
  , deleteOrigin(json.value("deleteOrigin", false))
{
}

ConvertStlToSvoCommand::~ConvertStlToSvoCommand() {}

std::string ConvertStlToSvoCommand::getType() const
{
    return "ConvertStlToSvo";
}

int ConvertStlToSvoCommand::getVersion() const
{
    return 1;
}

std::string ConvertStlToSvoCommand::getDescription() const
{
    return "Command: ConvertStlToSvo\n"
           "Version: 1\n"
           "Purpose: Converts a triangulation STL asset to a volumetric SVO asset.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"ConvertStlToSvo\",\n"
           "  \"version\": 1,\n"
           "  \"assetGuid\": <string>,\n"
           "  \"dimension\": <uint> (optional, default: 64),\n"
           "  \"deleteOrigin\": <bool> (optional, default: false)\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: None\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"ConvertStlToSvo\",\n"
           "  \"version\": 1,\n"
           "  \"status\": \"Success\" | \"AssetNotFound\" | \"NotTriangulationAsset\" | \"ConversionFailed\",\n"
           "  \"assetGuid\": <string>\n"
           "}";
}

nlohmann::json ConvertStlToSvoCommand::execute(Database& database, std::istream*, std::ostream*, bool recordUndo)
{
    auto baseAsset = database.getProject().getAsset(assetGuid);
    if (!baseAsset)
    {
        return {
            {      "type",       getType() },
            {   "version",    getVersion() },
            {    "status", "AssetNotFound" },
            { "assetGuid",              "" }
        };
    }

    auto triAsset = std::dynamic_pointer_cast<TriangulationAsset>(baseAsset);
    if (!triAsset)
    {
        return {
            {      "type",               getType() },
            {   "version",            getVersion() },
            {    "status", "NotTriangulationAsset" },
            { "assetGuid",                      "" }
        };
    }

    auto svoAsset = AssetConverter::stlToSvo(*triAsset, dimension);
    if (!svoAsset)
    {
        return {
            {      "type",          getType() },
            {   "version",       getVersion() },
            {    "status", "ConversionFailed" },
            { "assetGuid",                 "" }
        };
    }

    database.getProject().addAsset(svoAsset);

    if (deleteOrigin)
    {
        database.getProject().removeAsset(assetGuid);
        if (recordUndo)
        {
            database.getUndoStack().push(std::make_unique<ReplaceAssetUndoRecord>(baseAsset, svoAsset));
        }
    }
    else
    {
        if (recordUndo)
        {
            database.getUndoStack().push(std::make_unique<AddAssetUndoRecord>(svoAsset));
        }
    }

    return {
        {      "type",                      getType() },
        {   "version",                   getVersion() },
        {    "status",                      "Success" },
        { "assetGuid", svoAsset->getGuid().toString() }
    };
}

const CADacomb::GUID& ConvertStlToSvoCommand::getAssetGuid() const
{
    return assetGuid;
}

void ConvertStlToSvoCommand::setAssetGuid(const CADacomb::GUID& newGuid)
{
    assetGuid = newGuid;
}

uint32_t ConvertStlToSvoCommand::getDimension() const
{
    return dimension;
}

void ConvertStlToSvoCommand::setDimension(uint32_t newDimension)
{
    dimension = newDimension;
}

bool ConvertStlToSvoCommand::getDeleteOrigin() const
{
    return deleteOrigin;
}

void ConvertStlToSvoCommand::setDeleteOrigin(bool shouldDelete)
{
    deleteOrigin = shouldDelete;
}
