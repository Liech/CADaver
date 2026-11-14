#include "Commands/Selection/ClearSelectionCommand.h"

#include "Database/Database.h"

ClearSelectionCommand::ClearSelectionCommand() {}

ClearSelectionCommand::~ClearSelectionCommand() {}

std::string ClearSelectionCommand::getType() const
{
    return "ClearSelection";
}

int ClearSelectionCommand::getVersion() const
{
    return 1;
}

std::string ClearSelectionCommand::getDescription() const
{
    return "Command: ClearSelection\n"
           "Version: 1\n"
           "Purpose: Clears all current selections in the database.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"ClearSelection\",\n"
           "  \"version\": 1\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: None\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"ClearSelection\",\n"
           "  \"version\": 1\n"
           "}";
}

void ClearSelectionCommand::setInput(const Polyglot::Node& node) {}

nlohmann::json ClearSelectionCommand::execute(Database& database)
{
    database.getSelection().clear();
    return createResponse();
}
