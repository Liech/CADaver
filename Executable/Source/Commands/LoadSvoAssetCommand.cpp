#include "LoadSvoAssetCommand.h"

#include "CADacomb/Volume/SVO/SVOFile.h"
#include "Database/Database.h"
#include "Project/VoxelAsset.h"

LoadSvoAssetCommand::LoadSvoAssetCommand() {}

LoadSvoAssetCommand::LoadSvoAssetCommand(const std::string& name)
  : name(name)
{
}

LoadSvoAssetCommand::LoadSvoAssetCommand(const nlohmann::json& json)
  : name(json.at("name").get<std::string>())
{
}

LoadSvoAssetCommand::~LoadSvoAssetCommand() {}

std::string LoadSvoAssetCommand::getType() const
{
    return "LoadSvoAsset";
}

int LoadSvoAssetCommand::getVersion() const
{
    return 1;
}

nlohmann::json LoadSvoAssetCommand::execute(Database& database, std::istream* inputStream, std::ostream* outputStream)
{
    if (!inputStream)
    {
        return {
            {    "type",       getType() },
            { "version",    getVersion() },
            {  "status", "NoInputStream" },
            {   "index",              -1 },
            {    "name",            name }
        };
    }

    CADacomb::AABB3                      bounds;
    std::unique_ptr<CADacomb::SVO<bool>> svo = CADacomb::SVOFile::load<bool>(*inputStream, bounds);
    if (!svo)
    {
        return {
            {    "type",     getType() },
            { "version",  getVersion() },
            {  "status", "ParseFailed" },
            {   "index",            -1 },
            {    "name",          name }
        };
    }

    auto asset = std::make_shared<VoxelAsset>(name, "", std::move(svo), bounds);
    asset->markClean();
    database.getProject().addAsset(asset);
    int newIndex = static_cast<int>(database.getProject().getAssetCount()) - 1;
    database.getProject().setSelectedIndex(newIndex);

    return {
        {    "type",    getType() },
        { "version", getVersion() },
        {  "status",    "Success" },
        {   "index",     newIndex },
        {    "name",         name }
    };
}

const std::string& LoadSvoAssetCommand::getName() const
{
    return name;
}

void LoadSvoAssetCommand::setName(const std::string& newName)
{
    name = newName;
}
