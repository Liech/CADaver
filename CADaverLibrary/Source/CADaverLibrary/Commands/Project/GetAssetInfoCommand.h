#pragma once

#include "CADacomb/Util/GUID.h"
#include "Execution/Command.h"

// Command to query detailed information about an asset by GUID or name.
class GetAssetInfoCommand : public Command
{
  public:
    GetAssetInfoCommand();
    virtual ~GetAssetInfoCommand();

    virtual std::string getType() const override;
    virtual int         getVersion() const override;
    virtual std::string getDescription() const override;

    virtual void           setInput(const Polyglot::Node& node) override;
    virtual nlohmann::json execute(Database& database) override;

  private:
    CADacomb::GUID assetGuid;
    std::string    assetName;
};
