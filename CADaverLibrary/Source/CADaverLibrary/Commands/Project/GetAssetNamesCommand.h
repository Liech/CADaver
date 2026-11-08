#pragma once

#include "Execution/Command.h"

// Command to query all asset names currently in the project.
class GetAssetNamesCommand : public Command
{
  public:
    GetAssetNamesCommand();
    virtual ~GetAssetNamesCommand();

    virtual std::string getType() const override;
    virtual int         getVersion() const override;
    virtual std::string getDescription() const override;

    virtual nlohmann::json execute(Database& database) override;
};
