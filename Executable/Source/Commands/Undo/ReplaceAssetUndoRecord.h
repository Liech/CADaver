#pragma once

#include "Commands/Undo/UndoRecord.h"
#include "Database/Asset.h"

#include <memory>

class ReplaceAssetUndoRecord : public UndoRecord
{
  public:
    ReplaceAssetUndoRecord(std::shared_ptr<Asset> originAsset, size_t originIndex, std::shared_ptr<Asset> newAsset, size_t newIndex);
    virtual ~ReplaceAssetUndoRecord();

    void undo(Database& database) override;
    void redo(Database& database) override;

    std::string getDescription() const override;
    size_t      getSizeInBytes() const override;

    std::shared_ptr<Asset> getOriginAsset() const;
    std::shared_ptr<Asset> getNewAsset() const;
    size_t                 getOriginIndex() const;
    size_t                 getNewIndex() const;

  private:
    std::shared_ptr<Asset> originAsset;
    std::shared_ptr<Asset> newAsset;
    size_t                 originIndex = 0;
    size_t                 newIndex    = 0;
};
