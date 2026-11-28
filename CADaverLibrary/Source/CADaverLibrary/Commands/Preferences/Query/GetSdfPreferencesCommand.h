#pragma once

#include "Execution/Command.h"

// Queries current SDF sphere-tracing and shading preferences.
class GetSdfPreferencesCommand : public Command
{
  public:
    GetSdfPreferencesCommand();
    virtual ~GetSdfPreferencesCommand();

    virtual std::string    getType() const override;
    virtual int            getVersion() const override;
    virtual std::string    getDescription() const override;
    virtual nlohmann::json execute(Database& database) override;
};
