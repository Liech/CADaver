#pragma once

#include "CADacomb/Util/GUID.h"
#include "Execution/Command.h"

// Command to remove an asset from the project by GUID.
class RemoveAssetCommand : public Command
{
  public:
    RemoveAssetCommand();
    explicit RemoveAssetCommand(const CADacomb::GUID& assetGuid);
    explicit RemoveAssetCommand(const nlohmann::json& json);
    virtual ~RemoveAssetCommand();

    virtual std::string getType() const override;
    virtual int         getVersion() const override;
    virtual std::string getDescription() const override;

    virtual void           setInput(const nlohmann::json& json) override;
    virtual nlohmann::json execute(Database& database, std::istream* inputStream, std::ostream* outputStream, bool recordUndo = false) override;

    const CADacomb::GUID& getAssetGuid() const;
    void                  setAssetGuid(const CADacomb::GUID& guid);

  private:
    CADacomb::GUID assetGuid;
};
