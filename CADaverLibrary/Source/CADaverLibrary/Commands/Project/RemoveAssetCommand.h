#pragma once

#include "CADacomb/Util/GUID.h"
#include "Execution/Command.h"

#include <memory>

class Asset;

// Command to remove an asset from the project by GUID.
class RemoveAssetCommand : public Command
{
  public:
    RemoveAssetCommand();
    virtual ~RemoveAssetCommand();

    virtual std::string getType() const override;
    virtual int         getVersion() const override;
    virtual std::string getDescription() const override;

    virtual void           setInput(const nlohmann::json& json) override;
    virtual nlohmann::json execute(Database& database, std::istream* inputStream = nullptr, std::ostream* outputStream = nullptr) override;

    virtual bool   isUndoable() const override;
    virtual void   undo(Database& database) override;
    virtual void   redo(Database& database) override;
    virtual size_t getUndoSize() const override;

  private:
    CADacomb::GUID         assetGuid;
    std::shared_ptr<Asset> removedAsset;
};
