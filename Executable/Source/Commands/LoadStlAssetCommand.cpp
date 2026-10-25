#include "LoadStlAssetCommand.h"

#include "CADacomb/Triangle/Triangulation.h"
#include "Database/Database.h"
#include "Project/TriangulationAsset.h"

LoadStlAssetCommand::LoadStlAssetCommand() {}

LoadStlAssetCommand::LoadStlAssetCommand(const std::string& name)
  : name(name)
{
}

LoadStlAssetCommand::LoadStlAssetCommand(const nlohmann::json& json)
  : name(json.at("name").get<std::string>())
{
}

LoadStlAssetCommand::~LoadStlAssetCommand() {}

std::string LoadStlAssetCommand::getType() const
{
    return "LoadStlAsset";
}

int LoadStlAssetCommand::getVersion() const
{
    return 1;
}

nlohmann::json LoadStlAssetCommand::execute(Database& database, std::istream* inputStream, std::ostream* outputStream)
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

    auto triangulation = CADacomb::Triangulation::fromSTLStream(*inputStream);
    if (!triangulation || triangulation->indices.empty())
    {
        return {
            {    "type",     getType() },
            { "version",  getVersion() },
            {  "status", "ParseFailed" },
            {   "index",            -1 },
            {    "name",          name }
        };
    }

    auto asset = std::make_shared<TriangulationAsset>(name, std::move(triangulation));
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

const std::string& LoadStlAssetCommand::getName() const
{
    return name;
}

void LoadStlAssetCommand::setName(const std::string& newName)
{
    name = newName;
}
