#pragma once

#include "Command.h"

// Command to clear all assets from the project.
class ClearProjectCommand : public Command
{
  public:
    ClearProjectCommand();
    explicit ClearProjectCommand(const nlohmann::json& json);
    virtual ~ClearProjectCommand();

    virtual std::string getType() const override;
    virtual int         getVersion() const override;

    virtual nlohmann::json execute(Database& database, std::istream* inputStream, std::ostream* outputStream) override;
};
