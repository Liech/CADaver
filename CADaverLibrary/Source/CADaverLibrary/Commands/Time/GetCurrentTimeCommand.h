#pragma once

#include "Execution/Command.h"

// Query command returning the current project time.
class GetCurrentTimeCommand : public Command
{
  public:
    GetCurrentTimeCommand();
    virtual ~GetCurrentTimeCommand();

    std::string getType() const override;
    int         getVersion() const override;
    std::string getDescription() const override;

    nlohmann::json execute(Database& database) override;
};
