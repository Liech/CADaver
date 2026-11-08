#pragma once

#include "CADacomb/Util/GUID.h"
#include "Execution/Command.h"

// Command to query detailed state for a specific background task.
class GetTaskStateCommand : public Command
{
  public:
    GetTaskStateCommand();
    virtual ~GetTaskStateCommand();

    virtual std::string getType() const override;
    virtual int         getVersion() const override;
    virtual std::string getDescription() const override;

    virtual void           setInput(const Polyglot::Node& node) override;
    virtual nlohmann::json execute(Database& database) override;

  private:
    CADacomb::GUID taskGuid;
    CADacomb::GUID assetGuid;
};
