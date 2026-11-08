#pragma once

#include "Execution/Command.h"

// Command to query all active background tasks running in the project.
class GetActiveTasksCommand : public Command
{
  public:
    GetActiveTasksCommand();
    virtual ~GetActiveTasksCommand();

    virtual std::string getType() const override;
    virtual int         getVersion() const override;
    virtual std::string getDescription() const override;

    virtual nlohmann::json execute(Database& database) override;
};
