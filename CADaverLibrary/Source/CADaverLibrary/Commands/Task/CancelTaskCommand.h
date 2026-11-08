#pragma once

#include "CADacomb/Util/GUID.h"
#include "Execution/Command.h"

// Command to cancel an in-progress background task.
class CancelTaskCommand : public Command
{
  public:
    CancelTaskCommand();
    virtual ~CancelTaskCommand();

    virtual std::string getType() const override;
    virtual int         getVersion() const override;
    virtual std::string getDescription() const override;

    virtual void           setInput(const Polyglot::Node& node) override;
    virtual nlohmann::json execute(Database& database) override;

  private:
    CADacomb::GUID taskGuid;
    CADacomb::GUID assetGuid;
};
