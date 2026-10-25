#include "LoadStlAssetCommand.h"

#include "CADacomb/Triangle/Triangulation.h"
#include "Database/Database.h"
#include "Project/TriangulationAsset.h"

#include <cstdint>

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

    char header[80];
    inputStream->read(header, 80);

    uint32_t triangleCount = 0;
    inputStream->read(reinterpret_cast<char*>(&triangleCount), sizeof(uint32_t));

    if (!(*inputStream))
    {
        return {
            {    "type",          getType() },
            { "version",       getVersion() },
            {  "status", "HeaderReadFailed" },
            {   "index",                 -1 },
            {    "name",               name }
        };
    }

    auto triangulation = std::make_unique<CADacomb::Triangulation>();
    triangulation->vertices.reserve(triangleCount * 3);
    triangulation->indices.reserve(triangleCount * 3);

    for (uint32_t i = 0; i < triangleCount; i++)
    {
        float normal[3];
        inputStream->read(reinterpret_cast<char*>(normal), 12);

        for (int v = 0; v < 3; v++)
        {
            float vertex[3];
            inputStream->read(reinterpret_cast<char*>(vertex), 12);
            triangulation->indices.push_back(triangulation->vertices.size());
            triangulation->vertices.emplace_back(vertex[0], vertex[1], vertex[2]);
        }

        uint16_t attrByteCount = 0;
        inputStream->read(reinterpret_cast<char*>(&attrByteCount), 2);
    }

    if (!(*inputStream))
    {
        return {
            {    "type",         getType() },
            { "version",      getVersion() },
            {  "status", "TruncatedStream" },
            {   "index",                -1 },
            {    "name",              name }
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
