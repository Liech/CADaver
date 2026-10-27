#include "Commands/Undo/ReplaceAssetUndoRecord.h"

#include "Database/Database.h"

ReplaceAssetUndoRecord::ReplaceAssetUndoRecord(std::shared_ptr<Asset> originAssetInput, size_t originIndexInput, std::shared_ptr<Asset> newAssetInput, size_t newIndexInput)
  : originAsset(std::move(originAssetInput))
  , newAsset(std::move(newAssetInput))
  , originIndex(originIndexInput)
  , newIndex(newIndexInput)
{
}

ReplaceAssetUndoRecord::~ReplaceAssetUndoRecord() {}

void ReplaceAssetUndoRecord::undo(Database& database)
{
    auto& project = database.getProject();
    for (size_t i = 0; i < project.getAssetCount(); i++)
    {
        if (project.getAsset(i) == newAsset)
        {
            project.removeAsset(i);
            break;
        }
    }
    if (originAsset)
    {
        project.insertAsset(originIndex, originAsset);
    }
}

void ReplaceAssetUndoRecord::redo(Database& database)
{
    auto& project = database.getProject();
    if (originAsset)
    {
        for (size_t i = 0; i < project.getAssetCount(); i++)
        {
            if (project.getAsset(i) == originAsset)
            {
                project.removeAsset(i);
                break;
            }
        }
    }
    if (newAsset)
    {
        project.insertAsset(newIndex, newAsset);
    }
}

std::string ReplaceAssetUndoRecord::getDescription() const
{
    std::string originName = originAsset ? originAsset->getName() : "";
    std::string targetType = newAsset ? newAsset->getTypeName() : "Asset";
    return "Convert " + originName + " -> " + targetType;
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

size_t ReplaceAssetUndoRecord::getOriginIndex() const
{
    return originIndex;
}

size_t ReplaceAssetUndoRecord::getNewIndex() const
{
    return newIndex;
}
