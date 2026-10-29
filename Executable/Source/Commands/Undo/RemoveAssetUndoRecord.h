#pragma once

#include "Commands/Undo/UndoRecord.h"
#include "Database/Asset.h"

#include <memory>

class RemoveAssetUndoRecord : public UndoRecord
{
  public:
    RemoveAssetUndoRecord(std::shared_ptr<Asset> asset);
    virtual ~RemoveAssetUndoRecord();

    void undo(Database& database) override;
    void redo(Database& database) override;

    std::string getDescription() const override;
    size_t      getSizeInBytes() const override;

    std::shared_ptr<Asset> getAsset() const;

  private:
    std::shared_ptr<Asset> asset;
};
