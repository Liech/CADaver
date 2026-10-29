#include "Commands/Undo/AddAssetUndoRecord.h"

#include "Database/Database.h"

AddAssetUndoRecord::AddAssetUndoRecord(std::shared_ptr<Asset> assetInput)
  : asset(std::move(assetInput))
{
}

AddAssetUndoRecord::~AddAssetUndoRecord() {}

void AddAssetUndoRecord::undo(Database& database)
{
    if (asset)
    {
        database.getProject().removeAsset(asset->getGuid());
    }
}

void AddAssetUndoRecord::redo(Database& database)
{
    if (asset)
    {
        database.getProject().addAsset(asset);
    }
}

std::string AddAssetUndoRecord::getDescription() const
{
    std::string assetName = asset ? asset->getName() : "";
    return "Add " + (asset ? asset->getTypeName() : "Asset") + ": " + assetName;
}

size_t AddAssetUndoRecord::getSizeInBytes() const
{
    size_t size = sizeof(AddAssetUndoRecord);
    if (asset)
    {
        size += asset->getSizeInBytes();
    }
    return size;
}

std::shared_ptr<Asset> AddAssetUndoRecord::getAsset() const
{
    return asset;
}
