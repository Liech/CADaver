#pragma once

#include "Execution/Command.h"

// Command to query the current camera preferences.
class GetCameraPreferencesCommand : public Command
{
  public:
    GetCameraPreferencesCommand();
    virtual ~GetCameraPreferencesCommand();

    virtual std::string getType() const override;
    virtual int         getVersion() const override;
    virtual std::string getDescription() const override;

    virtual nlohmann::json execute(Database& database) override;
};
