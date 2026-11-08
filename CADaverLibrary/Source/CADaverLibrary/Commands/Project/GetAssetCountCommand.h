#pragma once

#include "Execution/Command.h"

// Command to query the total number of assets currently in the project.
class GetAssetCountCommand : public Command
{
  public:
    GetAssetCountCommand();
    virtual ~GetAssetCountCommand();

    virtual std::string getType() const override;
    virtual int         getVersion() const override;
    virtual std::string getDescription() const override;

    virtual nlohmann::json execute(Database& database) override;
};
