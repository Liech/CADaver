#include "SaveStlAssetCommand.h"

#include "CADacomb/Triangle/Triangulation.h"
#include "Database/Asset/TriangulationAsset.h"
#include "Database/Database.h"

SaveStlAssetCommand::SaveStlAssetCommand() {}

SaveStlAssetCommand::SaveStlAssetCommand(int assetIndex)
  : assetIndex(assetIndex)
{
}

SaveStlAssetCommand::SaveStlAssetCommand(const nlohmann::json& json)
  : assetIndex(json.at("assetIndex").get<int>())
{
}

SaveStlAssetCommand::~SaveStlAssetCommand() {}

std::string SaveStlAssetCommand::getType() const
{
    return "SaveStlAsset";
}

int SaveStlAssetCommand::getVersion() const
{
    return 1;
}

std::string SaveStlAssetCommand::getDescription() const
{
    return "Command: SaveStlAsset\n"
           "Version: 1\n"
           "Purpose: Saves a triangulation asset to an output stream in binary STL format.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"SaveStlAsset\",\n"
           "  \"version\": 1,\n"
           "  \"assetIndex\": <int>\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: Binary STL stream\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"SaveStlAsset\",\n"
           "  \"version\": 1,\n"
           "  \"status\": \"Success\" | \"NoOutputStream\" | \"AssetNotFound\" | \"NotTriangulationAsset\",\n"
           "  \"index\": <int>\n"
           "}";
}

nlohmann::json SaveStlAssetCommand::execute(Database& database, std::istream* inputStream, std::ostream* outputStream, bool)
{
    if (!outputStream)
    {
        return {
            {    "type",        getType() },
            { "version",     getVersion() },
            {  "status", "NoOutputStream" },
            {   "index",       assetIndex }
        };
    }

    auto baseAsset = database.getProject().getAsset(static_cast<size_t>(assetIndex));
    if (!baseAsset)
    {
        return {
            {    "type",       getType() },
            { "version",    getVersion() },
            {  "status", "AssetNotFound" },
            {   "index",      assetIndex }
        };
    }

    auto triAsset = std::dynamic_pointer_cast<TriangulationAsset>(baseAsset);
    if (!triAsset || !triAsset->getTriangulation())
    {
        return {
            {    "type",               getType() },
            { "version",            getVersion() },
            {  "status", "NotTriangulationAsset" },
            {   "index",              assetIndex }
        };
    }

    triAsset->getTriangulation()->saveAsSTL(*outputStream);

    return {
        {    "type",    getType() },
        { "version", getVersion() },
        {  "status",    "Success" },
        {   "index",   assetIndex }
    };
}

int SaveStlAssetCommand::getAssetIndex() const
{
    return assetIndex;
}

void SaveStlAssetCommand::setAssetIndex(int index)
{
    assetIndex = index;
}
