#include "Commands/Undo/AddAssetUndoRecord.h"

#include "Database/Database.h"

AddAssetUndoRecord::AddAssetUndoRecord(std::shared_ptr<Asset> assetInput, size_t indexInput)
  : asset(std::move(assetInput))
  , index(indexInput)
{
}

AddAssetUndoRecord::~AddAssetUndoRecord() {}

void AddAssetUndoRecord::undo(Database& database)
{
    database.getProject().removeAsset(index);
}

void AddAssetUndoRecord::redo(Database& database)
{
    database.getProject().insertAsset(index, asset);
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

size_t AddAssetUndoRecord::getIndex() const
{
    return index;
}
