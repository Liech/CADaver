#pragma once

#include "Execution/Command.h"

// Command to query the current render preferences.
class GetRenderPreferencesCommand : public Command
{
  public:
    GetRenderPreferencesCommand();
    virtual ~GetRenderPreferencesCommand();

    virtual std::string getType() const override;
    virtual int         getVersion() const override;
    virtual std::string getDescription() const override;

    virtual nlohmann::json execute(Database& database) override;
};
