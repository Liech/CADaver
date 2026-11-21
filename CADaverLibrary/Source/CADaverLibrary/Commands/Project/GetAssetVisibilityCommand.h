#pragma once

#include "CADacomb/Util/GUID.h"
#include "Execution/Command.h"

// Command to query the visibility state of an asset.
class GetAssetVisibilityCommand : public Command
{
  public:
    GetAssetVisibilityCommand();
    virtual ~GetAssetVisibilityCommand();

    virtual std::string getType() const override;
    virtual int         getVersion() const override;
    virtual std::string getDescription() const override;

    virtual void           setInput(const Polyglot::Node& node) override;
    virtual nlohmann::json execute(Database& database) override;

  private:
    CADacomb::GUID assetGuid;
};
