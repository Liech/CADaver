#include "ConvertSvoToStlCommand.h"

#include "Commands/Undo/AddAssetUndoRecord.h"
#include "Commands/Undo/ReplaceAssetUndoRecord.h"
#include "Database/Asset/TriangulationAsset.h"
#include "Database/Asset/VoxelAsset.h"
#include "Database/AssetConverter.h"
#include "Database/Database.h"

ConvertSvoToStlCommand::ConvertSvoToStlCommand() {}

ConvertSvoToStlCommand::ConvertSvoToStlCommand(int assetIndex, int targetDepth, bool deleteOrigin)
  : assetIndex(assetIndex)
  , targetDepth(targetDepth)
  , deleteOrigin(deleteOrigin)
{
}

ConvertSvoToStlCommand::ConvertSvoToStlCommand(const nlohmann::json& json)
  : assetIndex(json.at("assetIndex").get<int>())
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
           "Purpose: Converts a volumetric SVO asset into a blocky triangulation STL asset.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"ConvertSvoToStl\",\n"
           "  \"version\": 1,\n"
           "  \"assetIndex\": <int>,\n"
           "  \"targetDepth\": <int, optional, default -1>,\n"
           "  \"deleteOrigin\": <bool, optional, default false>\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: None\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"ConvertSvoToStl\",\n"
           "  \"version\": 1,\n"
           "  \"status\": \"Success\" | \"IndexOutOfBounds\" | \"NotVoxelAsset\" | \"ConversionFailed\",\n"
           "  \"index\": <int>\n"
           "}";
}

nlohmann::json ConvertSvoToStlCommand::execute(Database& database, std::istream*, std::ostream*, bool recordUndo)
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

    auto baseAsset  = database.getProject().getAsset(assetIndex);
    auto voxelAsset = std::dynamic_pointer_cast<VoxelAsset>(baseAsset);
    if (!voxelAsset)
    {
        return {
            {    "type",       getType() },
            { "version",    getVersion() },
            {  "status", "NotVoxelAsset" },
            {   "index",              -1 }
        };
    }

    auto stlAsset = AssetConverter::svoToStlBlocky(*voxelAsset, targetDepth);
    if (!stlAsset)
    {
        return {
            {    "type",          getType() },
            { "version",       getVersion() },
            {  "status", "ConversionFailed" },
            {   "index",                 -1 }
        };
    }

    database.getProject().addAsset(stlAsset);
    int newIndex = static_cast<int>(database.getProject().getAssetCount()) - 1;

    if (deleteOrigin)
    {
        database.getProject().removeAsset(static_cast<size_t>(assetIndex));
        newIndex--;
        if (recordUndo)
        {
            database.getUndoStack().push(std::make_unique<ReplaceAssetUndoRecord>(baseAsset, static_cast<size_t>(assetIndex), stlAsset, static_cast<size_t>(newIndex)));
        }
    }
    else
    {
        if (recordUndo)
        {
            database.getUndoStack().push(std::make_unique<AddAssetUndoRecord>(stlAsset, static_cast<size_t>(newIndex)));
        }
    }

    return {
        {    "type",    getType() },
        { "version", getVersion() },
        {  "status",    "Success" },
        {   "index",     newIndex }
    };
}

int ConvertSvoToStlCommand::getAssetIndex() const
{
    return assetIndex;
}

void ConvertSvoToStlCommand::setAssetIndex(int newIndex)
{
    assetIndex = newIndex;
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
