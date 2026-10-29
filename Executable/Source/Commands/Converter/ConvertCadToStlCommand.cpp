#include "Commands/Converter/ConvertCadToStlCommand.h"

#include "Commands/Undo/AddAssetUndoRecord.h"
#include "Commands/Undo/ReplaceAssetUndoRecord.h"
#include "Database/Asset/CadAsset.h"
#include "Database/Asset/TriangulationAsset.h"
#include "Database/AssetConverter.h"
#include "Database/Database.h"

ConvertCadToStlCommand::ConvertCadToStlCommand() {}

ConvertCadToStlCommand::ConvertCadToStlCommand(const CADacomb::GUID& assetGuid, double precision, bool deleteOrigin)
  : assetGuid(assetGuid)
  , precision(precision)
  , deleteOrigin(deleteOrigin)
{
}

ConvertCadToStlCommand::ConvertCadToStlCommand(const nlohmann::json& json)
  : assetGuid(json.value("assetGuid", ""))
  , precision(json.value("precision", 0.1))
  , deleteOrigin(json.value("deleteOrigin", false))
{
}

ConvertCadToStlCommand::~ConvertCadToStlCommand() {}

std::string ConvertCadToStlCommand::getType() const
{
    return "ConvertCadToStl";
}

int ConvertCadToStlCommand::getVersion() const
{
    return 1;
}

std::string ConvertCadToStlCommand::getDescription() const
{
    return "Command: ConvertCadToStl\n"
           "Version: 1\n"
           "Purpose: Converts a CAD asset to a triangulation STL asset.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"ConvertCadToStl\",\n"
           "  \"version\": 1,\n"
           "  \"assetGuid\": <string>,\n"
           "  \"precision\": <double> (optional, default: 0.1),\n"
           "  \"deleteOrigin\": <bool> (optional, default: false)\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: None\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"ConvertCadToStl\",\n"
           "  \"version\": 1,\n"
           "  \"status\": \"Success\" | \"AssetNotFound\" | \"NotCadAsset\" | \"ConversionFailed\",\n"
           "  \"assetGuid\": <string>\n"
           "}";
}

nlohmann::json ConvertCadToStlCommand::execute(Database& database, std::istream*, std::ostream*, bool recordUndo)
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

    auto cadAsset = std::dynamic_pointer_cast<CadAsset>(baseAsset);
    if (!cadAsset)
    {
        return {
            {      "type",     getType() },
            {   "version",  getVersion() },
            {    "status", "NotCadAsset" },
            { "assetGuid",            "" }
        };
    }

    auto stlAsset = AssetConverter::cadToStl(*cadAsset, precision);
    if (!stlAsset)
    {
        return {
            {      "type",          getType() },
            {   "version",       getVersion() },
            {    "status", "ConversionFailed" },
            { "assetGuid",                 "" }
        };
    }

    database.getProject().addAsset(stlAsset);

    if (deleteOrigin)
    {
        database.getProject().removeAsset(assetGuid);
        if (recordUndo)
        {
            database.getUndoStack().push(std::make_unique<ReplaceAssetUndoRecord>(baseAsset, stlAsset));
        }
    }
    else
    {
        if (recordUndo)
        {
            database.getUndoStack().push(std::make_unique<AddAssetUndoRecord>(stlAsset));
        }
    }

    return {
        {      "type",                      getType() },
        {   "version",                   getVersion() },
        {    "status",                      "Success" },
        { "assetGuid", stlAsset->getGuid().toString() }
    };
}

const CADacomb::GUID& ConvertCadToStlCommand::getAssetGuid() const
{
    return assetGuid;
}

void ConvertCadToStlCommand::setAssetGuid(const CADacomb::GUID& newGuid)
{
    assetGuid = newGuid;
}

double ConvertCadToStlCommand::getPrecision() const
{
    return precision;
}

void ConvertCadToStlCommand::setPrecision(double newPrecision)
{
    precision = newPrecision;
}

bool ConvertCadToStlCommand::getDeleteOrigin() const
{
    return deleteOrigin;
}

void ConvertCadToStlCommand::setDeleteOrigin(bool shouldDelete)
{
    deleteOrigin = shouldDelete;
}
