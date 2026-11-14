#pragma once

#include "CADacomb/Util/GUID.h"
#include "Execution/Command.h"

// Command to query the selection entry and subobjects for a specific asset GUID.
class GetAssetSelectionCommand : public Command
{
  public:
    GetAssetSelectionCommand();
    virtual ~GetAssetSelectionCommand();

    virtual std::string getType() const override;
    virtual int         getVersion() const override;
    virtual std::string getDescription() const override;

    virtual void           setInput(const Polyglot::Node& node) override;
    virtual nlohmann::json execute(Database& database) override;

  private:
    CADacomb::GUID assetGuid;
};
