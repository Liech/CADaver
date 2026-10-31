#include "Commands/Converter/ConvertSvoToStlCommand.h"

#include "CADacomb/Util/ProgressBar.h"
#include "Commands/Undo/AddAssetUndoRecord.h"
#include "Commands/Undo/ReplaceAssetUndoRecord.h"
#include "Commands/Undo/UndoStack.h"
#include "Conversion/AssetConverter.h"
#include "Database/Asset/TriangulationAsset.h"
#include "Database/Asset/VoxelAsset.h"
#include "Database/Database.h"

ConvertSvoToStlCommand::ConvertSvoToStlCommand() {}

ConvertSvoToStlCommand::ConvertSvoToStlCommand(const CADacomb::GUID& assetGuid, int targetDepth, bool deleteOrigin)
  : assetGuid(assetGuid)
  , targetDepth(targetDepth)
  , deleteOrigin(deleteOrigin)
{
}

ConvertSvoToStlCommand::ConvertSvoToStlCommand(const nlohmann::json& json)
  : assetGuid(json.value("assetGuid", ""))
  , targetDepth(json.value("targetDepth", -1))
  , deleteOrigin(json.value("deleteOrigin", false))
{
}

ConvertSvoToStlCommand::~ConvertSvoToStlCommand() {}

std::string ConvertSvoToStlCommand::getType() const
{
    return "ConvertSvoToStl";
}

int ConvertSvoToStlCommand::getVersion() const
{
    return 1;
}

std::string ConvertSvoToStlCommand::getDescription() const
{
    return "Command: ConvertSvoToStl\n"
           "Version: 1\n"
           "Purpose: Converts a volumetric SVO asset to a blocky triangulation STL asset.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"ConvertSvoToStl\",\n"
           "  \"version\": 1,\n"
           "  \"assetGuid\": <string>,\n"
           "  \"targetDepth\": <int> (optional, default: -1 for leaf depth),\n"
           "  \"deleteOrigin\": <bool> (optional, default: false)\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: None\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"ConvertSvoToStl\",\n"
           "  \"version\": 1,\n"
           "  \"status\": \"Success\" | \"AssetNotFound\" | \"NotVoxelAsset\" | \"ConversionFailed\",\n"
           "  \"assetGuid\": <string>\n"
           "}";
}

std::vector<CADacomb::GUID> ConvertSvoToStlCommand::getRequiredAssets() const
{
    return { assetGuid };
}

void ConvertSvoToStlCommand::setInput(const nlohmann::json& json)
{
    assetGuid    = json.value("assetGuid", "");
    targetDepth  = json.value("targetDepth", -1);
    deleteOrigin = json.value("deleteOrigin", false);
}

nlohmann::json ConvertSvoToStlCommand::execute(Database& database, std::istream*, std::ostream*, bool recordUndo)
{
    CADacomb::ProgressBar bar(1, "Converting SVO to STL");

    auto baseAsset = database.getProject().getAsset(assetGuid);
    if (!baseAsset || bar.isCanceled())
    {
        return createResponse("AssetNotFound",
                              {
                                { "assetGuid", "" }
        });
    }

    auto voxelAsset = std::dynamic_pointer_cast<VoxelAsset>(baseAsset);
    if (!voxelAsset || bar.isCanceled())
    {
        return createResponse("NotVoxelAsset",
                              {
                                { "assetGuid", "" }
        });
    }

    auto stlAsset = AssetConverter::svoToStlBlocky(*voxelAsset, targetDepth);
    if (!stlAsset || bar.isCanceled())
    {
        return createResponse("ConversionFailed",
                              {
                                { "assetGuid", "" }
        });
    }

    bar.addProgress(1);

    database.getProject().addAsset(stlAsset);

    if (deleteOrigin)
    {
        database.getProject().removeAsset(assetGuid);
        if (recordUndo)
        {
            UndoStack::getInstance().push(std::make_unique<ReplaceAssetUndoRecord>(baseAsset, stlAsset));
        }
    }
    else
    {
        if (recordUndo)
        {
            UndoStack::getInstance().push(std::make_unique<AddAssetUndoRecord>(stlAsset));
        }
    }

    return createResponse("Success",
                          {
                            { "assetGuid", stlAsset->getGuid().toString() }
    });
}

const CADacomb::GUID& ConvertSvoToStlCommand::getAssetGuid() const
{
    return assetGuid;
}

void ConvertSvoToStlCommand::setAssetGuid(const CADacomb::GUID& newGuid)
{
    assetGuid = newGuid;
}

int ConvertSvoToStlCommand::getTargetDepth() const
{
    return targetDepth;
}

void ConvertSvoToStlCommand::setTargetDepth(int newDepth)
{
    targetDepth = newDepth;
}

bool ConvertSvoToStlCommand::getDeleteOrigin() const
{
    return deleteOrigin;
}

void ConvertSvoToStlCommand::setDeleteOrigin(bool shouldDelete)
{
    deleteOrigin = shouldDelete;
}
