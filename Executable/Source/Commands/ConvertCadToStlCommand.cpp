#include "ConvertCadToStlCommand.h"

#include "Database/Asset/CadAsset.h"
#include "Database/Asset/TriangulationAsset.h"
#include "Database/AssetConverter.h"
#include "Database/Database.h"

ConvertCadToStlCommand::ConvertCadToStlCommand() {}

ConvertCadToStlCommand::ConvertCadToStlCommand(int assetIndex)
  : assetIndex(assetIndex)
{
}

ConvertCadToStlCommand::ConvertCadToStlCommand(const nlohmann::json& json)
  : assetIndex(json.at("assetIndex").get<int>())
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
           "Purpose: Converts a CAD asset into a triangulation STL asset.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"ConvertCadToStl\",\n"
           "  \"version\": 1,\n"
           "  \"assetIndex\": <int>\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: None\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"ConvertCadToStl\",\n"
           "  \"version\": 1,\n"
           "  \"status\": \"Success\" | \"IndexOutOfBounds\" | \"NotCadAsset\" | \"ConversionFailed\",\n"
           "  \"index\": <int>\n"
           "}";
}

nlohmann::json ConvertCadToStlCommand::execute(Database& database, std::istream* inputStream, std::ostream* outputStream)
{
    if (assetIndex < 0 || static_cast<size_t>(assetIndex) >= database.getProject().getAssetCount())
    {
        return {
            {    "type",          getType() },
            { "version",       getVersion() },
            {  "status", "IndexOutOfBounds" },
            {   "index",                 -1 }
        };
    }

    auto baseAsset = database.getProject().getAsset(assetIndex);
    auto cadAsset  = std::dynamic_pointer_cast<CadAsset>(baseAsset);
    if (!cadAsset)
    {
        return {
            {    "type",     getType() },
            { "version",  getVersion() },
            {  "status", "NotCadAsset" },
            {   "index",            -1 }
        };
    }

    auto stlAsset = AssetConverter::cadToStl(*cadAsset);
    if (!stlAsset)
    {
        return {
            {    "type",          getType() },
            { "version",       getVersion() },
            {  "status", "ConversionFailed" },
            {   "index",                 -1 }
        };
    }

    database.getProject().addAsset(stlAsset);
    int newIndex = static_cast<int>(database.getProject().getAssetCount()) - 1;

    return {
        {    "type",    getType() },
        { "version", getVersion() },
        {  "status",    "Success" },
        {   "index",     newIndex }
    };
}

int ConvertCadToStlCommand::getAssetIndex() const
{
    return assetIndex;
}

void ConvertCadToStlCommand::setAssetIndex(int newIndex)
{
    assetIndex = newIndex;
}
