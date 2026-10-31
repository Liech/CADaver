#pragma once

#include "Execution/Command.h"

class UndoCommand : public Command
{
  public:
    UndoCommand();
    UndoCommand(const nlohmann::json& json);
    virtual ~UndoCommand();

    std::string getType() const override;
    int         getVersion() const override;
    std::string getDescription() const override;

    nlohmann::json execute(Database& database, std::istream* inputStream, std::ostream* outputStream, bool recordUndo = false) override;
};
