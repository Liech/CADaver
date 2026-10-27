#pragma once

#include "Commands/Undo/UndoRecord.h"
#include "Database/Asset.h"

#include <memory>

class AddAssetUndoRecord : public UndoRecord
{
  public:
    AddAssetUndoRecord(std::shared_ptr<Asset> asset, size_t index);
    virtual ~AddAssetUndoRecord();

    void undo(Database& database) override;
    void redo(Database& database) override;

    std::string getDescription() const override;
    size_t      getSizeInBytes() const override;

    std::shared_ptr<Asset> getAsset() const;
    size_t                 getIndex() const;

  private:
    std::shared_ptr<Asset> asset;
    size_t                 index = 0;
};
