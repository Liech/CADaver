#include "ClearProjectCommand.h"

#include "Database/Database.h"

ClearProjectCommand::ClearProjectCommand() {}

ClearProjectCommand::ClearProjectCommand(const nlohmann::json& json) {}

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

nlohmann::json ClearProjectCommand::execute(Database& database, std::istream* inputStream, std::ostream* outputStream)
{
    database.getProject().clear();

    return {
        {    "type",    getType() },
        { "version", getVersion() },
        {  "status",    "Success" }
    };
}
