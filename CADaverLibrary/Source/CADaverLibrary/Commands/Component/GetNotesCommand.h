#pragma once

#include "CADacomb/Util/GUID.h"
#include "Execution/Command.h"

// Command to query all text note components attached to a specified asset.
class GetNotesCommand : public Command
{
  public:
    GetNotesCommand();
    virtual ~GetNotesCommand();

    virtual std::string getType() const override;
    virtual int         getVersion() const override;
    virtual std::string getDescription() const override;

    virtual void           setInput(const Polyglot::Node& node) override;
    virtual nlohmann::json execute(Database& database) override;

  private:
    CADacomb::GUID assetGuid;
};
