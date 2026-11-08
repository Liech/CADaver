#pragma once

#include "Execution/Command.h"

// Command to query the current user interface preferences.
class GetUIPreferencesCommand : public Command
{
  public:
    GetUIPreferencesCommand();
    virtual ~GetUIPreferencesCommand();

    virtual std::string getType() const override;
    virtual int         getVersion() const override;
    virtual std::string getDescription() const override;

    virtual nlohmann::json execute(Database& database) override;
};
