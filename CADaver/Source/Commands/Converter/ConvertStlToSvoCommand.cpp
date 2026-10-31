#include "Commands/Converter/ConvertStlToSvoCommand.h"

#include "CADacomb/Util/ProgressBar.h"
#include "Conversion/AssetConverter.h"
#include "Database/Asset/TriangulationAsset.h"
#include "Database/Asset/VoxelAsset.h"
#include "Database/Database.h"

ConvertStlToSvoCommand::ConvertStlToSvoCommand() {}

ConvertStlToSvoCommand::ConvertStlToSvoCommand(const CADacomb::GUID& assetGuid, uint32_t dimension, bool deleteOrigin)
  : assetGuid(assetGuid)
  , dimension(dimension)
  , deleteOrigin(deleteOrigin)
{
}

ConvertStlToSvoCommand::ConvertStlToSvoCommand(const nlohmann::json& json)
  : assetGuid(json.value("assetGuid", ""))
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
           "Purpose: Converts a triangulation STL asset to a volumetric SVO asset.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"ConvertStlToSvo\",\n"
           "  \"version\": 1,\n"
           "  \"assetGuid\": <string>,\n"
           "  \"dimension\": <uint> (optional, default: 64),\n"
           "  \"deleteOrigin\": <bool> (optional, default: false)\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: None\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"ConvertStlToSvo\",\n"
           "  \"version\": 1,\n"
           "  \"status\": \"Success\" | \"AssetNotFound\" | \"NotTriangulationAsset\" | \"ConversionFailed\",\n"
           "  \"assetGuid\": <string>\n"
           "}";
}

std::vector<CADacomb::GUID> ConvertStlToSvoCommand::getRequiredAssets() const
{
    return { assetGuid };
}

void ConvertStlToSvoCommand::setInput(const nlohmann::json& json)
{
    assetGuid    = json.value("assetGuid", "");
    dimension    = json.value("dimension", 64u);
    deleteOrigin = json.value("deleteOrigin", false);
}

nlohmann::json ConvertStlToSvoCommand::execute(Database& database, std::istream*, std::ostream*)
{
    CADacomb::ProgressBar bar(1, "Converting STL to SVO");

    auto baseAsset = database.getProject().getAsset(assetGuid);
    if (!baseAsset || bar.isCanceled())
    {
        return createResponse("AssetNotFound",
                              {
                                { "assetGuid", "" }
        });
    }

    auto triAsset = std::dynamic_pointer_cast<TriangulationAsset>(baseAsset);
    if (!triAsset || bar.isCanceled())
    {
        return createResponse("NotTriangulationAsset",
                              {
                                { "assetGuid", "" }
        });
    }

    auto svoAsset = AssetConverter::stlToSvo(*triAsset, dimension);
    if (!svoAsset || bar.isCanceled())
    {
        return createResponse("ConversionFailed",
                              {
                                { "assetGuid", "" }
        });
    }

    bar.addProgress(1);

    database.getProject().addAsset(svoAsset);
    createdAsset = svoAsset;

    if (deleteOrigin)
    {
        originAsset = baseAsset;
        database.getProject().removeAsset(assetGuid);
    }

    return createResponse("Success",
                          {
                            { "assetGuid", svoAsset->getGuid().toString() }
    });
}

bool ConvertStlToSvoCommand::isUndoable() const
{
    return createdAsset != nullptr;
}

void ConvertStlToSvoCommand::undo(Database& database)
{
    if (createdAsset)
    {
        database.getProject().removeAsset(createdAsset->getGuid());
    }
    if (deleteOrigin && originAsset)
    {
        database.getProject().addAsset(originAsset);
    }
}

void ConvertStlToSvoCommand::redo(Database& database)
{
    if (deleteOrigin && originAsset)
    {
        database.getProject().removeAsset(originAsset->getGuid());
    }
    if (createdAsset)
    {
        database.getProject().addAsset(createdAsset);
    }
}

size_t ConvertStlToSvoCommand::getUndoSize() const
{
    size_t size = sizeof(ConvertStlToSvoCommand);
    if (originAsset)
    {
        size += originAsset->getSizeInBytes();
    }
    if (createdAsset)
    {
        size += createdAsset->getSizeInBytes();
    }
    return size;
}

const CADacomb::GUID& ConvertStlToSvoCommand::getAssetGuid() const
{
    return assetGuid;
}

void ConvertStlToSvoCommand::setAssetGuid(const CADacomb::GUID& newGuid)
{
    assetGuid = newGuid;
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
