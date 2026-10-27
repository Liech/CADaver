#include "ConvertStlToSvoCommand.h"

#include "Database/Asset/TriangulationAsset.h"
#include "Database/Asset/VoxelAsset.h"
#include "Database/AssetConverter.h"
#include "Database/Database.h"

ConvertStlToSvoCommand::ConvertStlToSvoCommand() {}

ConvertStlToSvoCommand::ConvertStlToSvoCommand(int assetIndex, uint32_t dimension, bool deleteOrigin)
  : assetIndex(assetIndex)
  , dimension(dimension)
  , deleteOrigin(deleteOrigin)
{
}

ConvertStlToSvoCommand::ConvertStlToSvoCommand(const nlohmann::json& json)
  : assetIndex(json.at("assetIndex").get<int>())
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
           "Purpose: Converts a triangulation STL asset into a volumetric SVO asset.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"ConvertStlToSvo\",\n"
           "  \"version\": 1,\n"
           "  \"assetIndex\": <int>,\n"
           "  \"dimension\": <uint, optional, default 64>,\n"
           "  \"deleteOrigin\": <bool, optional, default false>\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: None\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"ConvertStlToSvo\",\n"
           "  \"version\": 1,\n"
           "  \"status\": \"Success\" | \"IndexOutOfBounds\" | \"NotTriangulationAsset\" | \"ConversionFailed\",\n"
           "  \"index\": <int>\n"
           "}";
}

nlohmann::json ConvertStlToSvoCommand::execute(Database& database, std::istream* inputStream, std::ostream* outputStream)
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
    auto triAsset  = std::dynamic_pointer_cast<TriangulationAsset>(baseAsset);
    if (!triAsset)
    {
        return {
            {    "type",               getType() },
            { "version",            getVersion() },
            {  "status", "NotTriangulationAsset" },
            {   "index",                      -1 }
        };
    }

    auto svoAsset = AssetConverter::stlToSvo(*triAsset, dimension);
    if (!svoAsset)
    {
        return {
            {    "type",          getType() },
            { "version",       getVersion() },
            {  "status", "ConversionFailed" },
            {   "index",                 -1 }
        };
    }

    database.getProject().addAsset(svoAsset);
    int newIndex = static_cast<int>(database.getProject().getAssetCount()) - 1;

    if (deleteOrigin)
    {
        database.getProject().removeAsset(static_cast<size_t>(assetIndex));
        newIndex--;
    }

    return {
        {    "type",    getType() },
        { "version", getVersion() },
        {  "status",    "Success" },
        {   "index",     newIndex }
    };
}

int ConvertStlToSvoCommand::getAssetIndex() const
{
    return assetIndex;
}

void ConvertStlToSvoCommand::setAssetIndex(int newIndex)
{
    assetIndex = newIndex;
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
