#include "SaveStlAssetCommand.h"

#include "CADacomb/Triangle/Triangulation.h"
#include "Database/Database.h"
#include "Project/TriangulationAsset.h"

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

nlohmann::json SaveStlAssetCommand::execute(Database& database, std::istream* inputStream, std::ostream* outputStream)
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
