#include "Commands/Project/ClearProjectCommand.h"

#include "Commands/Undo/ClearProjectUndoRecord.h"
#include "Commands/Undo/UndoStack.h"
#include "Database/Database.h"

ClearProjectCommand::ClearProjectCommand() {}

ClearProjectCommand::ClearProjectCommand(const nlohmann::json&) {}

ClearProjectCommand::~ClearProjectCommand() {}

std::string ClearProjectCommand::getType() const
{
    return "ClearProject";
}

int ClearProjectCommand::getVersion() const
{
    return 1;
}

std::string ClearProjectCommand::getDescription() const
{
    return "Command: ClearProject\n"
           "Version: 1\n"
           "Purpose: Clears all assets from the current project.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"ClearProject\",\n"
           "  \"version\": 1\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: None\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"ClearProject\",\n"
           "  \"version\": 1,\n"
           "  \"status\": \"Success\"\n"
           "}";
}

nlohmann::json ClearProjectCommand::execute(Database& database, std::istream*, std::ostream*, bool recordUndo)
{
    auto assets = database.getProject().getAssets();

    database.getProject().clear();

    if (recordUndo && !assets.empty())
    {
        UndoStack::getInstance().push(std::make_unique<ClearProjectUndoRecord>(assets));
    }

    return {
        {    "type",    getType() },
        { "version", getVersion() },
        {  "status",    "Success" }
    };
}
