#include "Commands/Undo/RemoveAssetUndoRecord.h"

#include "Database/Database.h"

RemoveAssetUndoRecord::RemoveAssetUndoRecord(std::shared_ptr<Asset> assetInput, size_t indexInput)
  : asset(std::move(assetInput))
  , index(indexInput)
{
}

RemoveAssetUndoRecord::~RemoveAssetUndoRecord() {}

void RemoveAssetUndoRecord::undo(Database& database)
{
    database.getProject().insertAsset(index, asset);
}

void RemoveAssetUndoRecord::redo(Database& database)
{
    database.getProject().removeAsset(index);
}

std::string RemoveAssetUndoRecord::getDescription() const
{
    std::string assetName = asset ? asset->getName() : "";
    return "Remove " + (asset ? asset->getTypeName() : "Asset") + ": " + assetName;
}

size_t RemoveAssetUndoRecord::getSizeInBytes() const
{
    size_t size = sizeof(RemoveAssetUndoRecord);
    if (asset)
    {
        size += asset->getSizeInBytes();
    }
    return size;
}

std::shared_ptr<Asset> RemoveAssetUndoRecord::getAsset() const
{
    return asset;
}

size_t RemoveAssetUndoRecord::getIndex() const
{
    return index;
}
