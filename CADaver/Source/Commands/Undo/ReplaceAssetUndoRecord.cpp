#include "Commands/Undo/ReplaceAssetUndoRecord.h"

#include "Database/Database.h"

ReplaceAssetUndoRecord::ReplaceAssetUndoRecord(std::shared_ptr<Asset> originAssetInput, std::shared_ptr<Asset> newAssetInput)
  : originAsset(std::move(originAssetInput))
  , newAsset(std::move(newAssetInput))
{
}

ReplaceAssetUndoRecord::~ReplaceAssetUndoRecord() {}

void ReplaceAssetUndoRecord::undo(Database& database)
{
    if (newAsset)
    {
        database.getProject().removeAsset(newAsset->getGuid());
    }
    if (originAsset)
    {
        database.getProject().addAsset(originAsset);
    }
}

void ReplaceAssetUndoRecord::redo(Database& database)
{
    if (originAsset)
    {
        database.getProject().removeAsset(originAsset->getGuid());
    }
    if (newAsset)
    {
        database.getProject().addAsset(newAsset);
    }
}

std::string ReplaceAssetUndoRecord::getDescription() const
{
    std::string originName = originAsset ? originAsset->getName() : "";
    std::string newName    = newAsset ? newAsset->getName() : "";
    return "Replace Asset: " + originName + " with " + newName;
}

size_t ReplaceAssetUndoRecord::getSizeInBytes() const
{
    size_t size = sizeof(ReplaceAssetUndoRecord);
    if (originAsset)
    {
        size += originAsset->getSizeInBytes();
    }
    if (newAsset)
    {
        size += newAsset->getSizeInBytes();
    }
    return size;
}

std::shared_ptr<Asset> ReplaceAssetUndoRecord::getOriginAsset() const
{
    return originAsset;
}

std::shared_ptr<Asset> ReplaceAssetUndoRecord::getNewAsset() const
{
    return newAsset;
}
