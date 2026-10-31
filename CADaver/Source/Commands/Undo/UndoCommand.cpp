#include "Commands/Undo/UndoCommand.h"

#include "Commands/Undo/UndoStack.h"
#include "Database/Database.h"

UndoCommand::UndoCommand() {}

UndoCommand::UndoCommand(const nlohmann::json&) {}

UndoCommand::~UndoCommand() {}

std::string UndoCommand::getType() const
{
    return "Undo";
}

int UndoCommand::getVersion() const
{
    return 1;
}

std::string UndoCommand::getDescription() const
{
    return "Command: Undo\n"
           "Version: 1\n"
           "Purpose: Undoes the last recorded mutation in the database.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"Undo\",\n"
           "  \"version\": 1\n"
           "}\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"Undo\",\n"
           "  \"version\": 1,\n"
           "  \"status\": \"Success\" | \"NothingToUndo\",\n"
           "  \"canUndo\": <bool>,\n"
           "  \"canRedo\": <bool>\n"
           "}";
}

nlohmann::json UndoCommand::execute(Database& database, std::istream*, std::ostream*)
{
    if (!UndoStack::getInstance().canUndo())
    {
        return {
            {    "type",                          getType() },
            { "version",                       getVersion() },
            {  "status",                      "NotExecuted" },
            { "canUndo",                              false },
            { "canRedo", UndoStack::getInstance().canRedo() }
        };
    }

    UndoStack::getInstance().undo(database);

    return {
        {    "type",                          getType() },
        { "version",                       getVersion() },
        {  "status",                          "Success" },
        { "canUndo", UndoStack::getInstance().canUndo() },
        { "canRedo", UndoStack::getInstance().canRedo() }
    };
}
