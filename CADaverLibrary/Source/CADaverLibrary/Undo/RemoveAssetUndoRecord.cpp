#include "Undo/RemoveAssetUndoRecord.h"

#include "Database/Database.h"

RemoveAssetUndoRecord::RemoveAssetUndoRecord(std::shared_ptr<Asset> assetInput)
  : asset(std::move(assetInput))
{
}

RemoveAssetUndoRecord::~RemoveAssetUndoRecord() {}

void RemoveAssetUndoRecord::undo(Database& database)
{
    if (asset)
    {
        database.getProject().addAsset(asset);
    }
}

void RemoveAssetUndoRecord::redo(Database& database)
{
    if (asset)
    {
        database.getProject().removeAsset(asset->getGuid());
    }
}

std::string RemoveAssetUndoRecord::getDescription() const
{
    std::string assetName = asset ? asset->getName() : "";
    return "Remove " + (asset ? asset->getType() : "Asset") + ": " + assetName;
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
