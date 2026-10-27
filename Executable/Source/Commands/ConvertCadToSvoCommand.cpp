#include "ConvertCadToSvoCommand.h"

#include "Database/Asset/CadAsset.h"
#include "Database/Asset/VoxelAsset.h"
#include "Database/AssetConverter.h"
#include "Database/Database.h"

ConvertCadToSvoCommand::ConvertCadToSvoCommand() {}

ConvertCadToSvoCommand::ConvertCadToSvoCommand(int assetIndex, double precision, uint32_t dimension, bool deleteOrigin)
  : assetIndex(assetIndex)
  , precision(precision)
  , dimension(dimension)
  , deleteOrigin(deleteOrigin)
{
}

ConvertCadToSvoCommand::ConvertCadToSvoCommand(const nlohmann::json& json)
  : assetIndex(json.at("assetIndex").get<int>())
  , precision(json.value("precision", 0.1))
  , dimension(json.value("dimension", 64u))
  , deleteOrigin(json.value("deleteOrigin", false))
{
}

ConvertCadToSvoCommand::~ConvertCadToSvoCommand() {}

std::string ConvertCadToSvoCommand::getType() const
{
    return "ConvertCadToSvo";
}

int ConvertCadToSvoCommand::getVersion() const
{
    return 1;
}

std::string ConvertCadToSvoCommand::getDescription() const
{
    return "Command: ConvertCadToSvo\n"
           "Version: 1\n"
           "Purpose: Converts a CAD asset into a volumetric SVO asset.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"ConvertCadToSvo\",\n"
           "  \"version\": 1,\n"
           "  \"assetIndex\": <int>,\n"
           "  \"precision\": <double, optional, default 0.1>,\n"
           "  \"dimension\": <uint, optional, default 64>,\n"
           "  \"deleteOrigin\": <bool, optional, default false>\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: None\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"ConvertCadToSvo\",\n"
           "  \"version\": 1,\n"
           "  \"status\": \"Success\" | \"IndexOutOfBounds\" | \"NotCadAsset\" | \"ConversionFailed\",\n"
           "  \"index\": <int>\n"
           "}";
}

nlohmann::json ConvertCadToSvoCommand::execute(Database& database, std::istream* inputStream, std::ostream* outputStream)
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

    auto svoAsset = AssetConverter::cadToSvo(*cadAsset, precision, dimension);
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

int ConvertCadToSvoCommand::getAssetIndex() const
{
    return assetIndex;
}

void ConvertCadToSvoCommand::setAssetIndex(int newIndex)
{
    assetIndex = newIndex;
}

double ConvertCadToSvoCommand::getPrecision() const
{
    return precision;
}

void ConvertCadToSvoCommand::setPrecision(double newPrecision)
{
    precision = newPrecision;
}

uint32_t ConvertCadToSvoCommand::getDimension() const
{
    return dimension;
}

void ConvertCadToSvoCommand::setDimension(uint32_t newDimension)
{
    dimension = newDimension;
}

bool ConvertCadToSvoCommand::getDeleteOrigin() const
{
    return deleteOrigin;
}

void ConvertCadToSvoCommand::setDeleteOrigin(bool shouldDelete)
{
    deleteOrigin = shouldDelete;
}
