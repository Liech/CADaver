#pragma once

#include "Commands/Command.h"

class RedoCommand : public Command
{
  public:
    RedoCommand();
    RedoCommand(const nlohmann::json& json);
    virtual ~RedoCommand();

    std::string getType() const override;
    int         getVersion() const override;
    std::string getDescription() const override;

    nlohmann::json execute(Database& database, std::istream* inputStream, std::ostream* outputStream, bool recordUndo = false) override;
};
