#pragma once

#include "Database/Asset.h"
#include "Undo/UndoRecord.h"

#include <memory>

class AddAssetUndoRecord : public UndoRecord
{
  public:
    AddAssetUndoRecord(std::shared_ptr<Asset> asset);
    virtual ~AddAssetUndoRecord();

    void undo(Database& database) override;
    void redo(Database& database) override;

    std::string getDescription() const override;
    size_t      getSizeInBytes() const override;

    std::shared_ptr<Asset> getAsset() const;

  private:
    std::shared_ptr<Asset> asset;
};
