#include "Commands/Undo/RedoCommand.h"

#include "Commands/Undo/UndoStack.h"
#include "Database/Database.h"

RedoCommand::RedoCommand() {}

RedoCommand::RedoCommand(const nlohmann::json&) {}

RedoCommand::~RedoCommand() {}

std::string RedoCommand::getType() const
{
    return "Redo";
}

int RedoCommand::getVersion() const
{
    return 1;
}

std::string RedoCommand::getDescription() const
{
    return "Command: Redo\n"
           "Version: 1\n"
           "Purpose: Redoes the last undone mutation in the database.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"Redo\",\n"
           "  \"version\": 1\n"
           "}\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"Redo\",\n"
           "  \"version\": 1,\n"
           "  \"status\": \"Success\" | \"NothingToRedo\",\n"
           "  \"canUndo\": <bool>,\n"
           "  \"canRedo\": <bool>\n"
           "}";
}

nlohmann::json RedoCommand::execute(Database& database, std::istream*, std::ostream*, bool)
{
    if (!UndoStack::getInstance().canRedo())
    {
        return {
            {    "type",                          getType() },
            { "version",                       getVersion() },
            {  "status",                      "NotExecuted" },
            { "canUndo", UndoStack::getInstance().canUndo() },
            { "canRedo",                              false }
        };
    }

    UndoStack::getInstance().redo(database);

    return {
        {    "type",                          getType() },
        { "version",                       getVersion() },
        {  "status",                          "Success" },
        { "canUndo", UndoStack::getInstance().canUndo() },
        { "canRedo", UndoStack::getInstance().canRedo() }
    };
}
