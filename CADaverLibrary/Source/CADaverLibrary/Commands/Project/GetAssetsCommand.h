#pragma once

#include "Execution/Command.h"

// Command to query all assets and their attached component summaries in the project.
class GetAssetsCommand : public Command
{
  public:
    GetAssetsCommand();
    virtual ~GetAssetsCommand();

    virtual std::string getType() const override;
    virtual int         getVersion() const override;
    virtual std::string getDescription() const override;

    virtual nlohmann::json execute(Database& database) override;
};
