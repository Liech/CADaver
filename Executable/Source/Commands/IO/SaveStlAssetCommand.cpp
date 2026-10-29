#include "Commands/IO/SaveStlAssetCommand.h"

#include "Database/Asset/TriangulationAsset.h"
#include "Database/Database.h"

SaveStlAssetCommand::SaveStlAssetCommand() {}

SaveStlAssetCommand::SaveStlAssetCommand(const CADacomb::GUID& assetGuid)
  : assetGuid(assetGuid)
{
}

SaveStlAssetCommand::SaveStlAssetCommand(const nlohmann::json& json)
  : assetGuid(json.value("assetGuid", ""))
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
           "Purpose: Saves a triangulation asset to a binary STL output stream.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"SaveStlAsset\",\n"
           "  \"version\": 1,\n"
           "  \"assetGuid\": <string>\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: Binary STL data\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"SaveStlAsset\",\n"
           "  \"version\": 1,\n"
           "  \"status\": \"Success\" | \"AssetNotFound\" | \"NotTriangulationAsset\" | \"NoTriangulation\" | \"NoOutputStream\"\n"
           "}";
}

nlohmann::json SaveStlAssetCommand::execute(Database& database, std::istream*, std::ostream* outputStream, bool)
{
    auto baseAsset = database.getProject().getAsset(assetGuid);
    if (!baseAsset)
    {
        return {
            {    "type",       getType() },
            { "version",    getVersion() },
            {  "status", "AssetNotFound" }
        };
    }

    auto triAsset = std::dynamic_pointer_cast<TriangulationAsset>(baseAsset);
    if (!triAsset)
    {
        return {
            {    "type",               getType() },
            { "version",            getVersion() },
            {  "status", "NotTriangulationAsset" }
        };
    }

    auto* tri = triAsset->getTriangulation();
    if (!tri)
    {
        return {
            {    "type",         getType() },
            { "version",      getVersion() },
            {  "status", "NoTriangulation" }
        };
    }

    if (!outputStream)
    {
        return {
            {    "type",        getType() },
            { "version",     getVersion() },
            {  "status", "NoOutputStream" }
        };
    }

    tri->saveAsSTL(*outputStream);

    return {
        {    "type",    getType() },
        { "version", getVersion() },
        {  "status",    "Success" }
    };
}

const CADacomb::GUID& SaveStlAssetCommand::getAssetGuid() const
{
    return assetGuid;
}

void SaveStlAssetCommand::setAssetGuid(const CADacomb::GUID& guid)
{
    assetGuid = guid;
}
