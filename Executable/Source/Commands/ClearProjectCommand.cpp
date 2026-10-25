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

nlohmann::json ClearProjectCommand::execute(Database& database, std::istream* inputStream, std::ostream* outputStream)
{
    database.getProject().clear();

    return {
        {    "type",    getType() },
        { "version", getVersion() },
        {  "status",    "Success" }
    };
}
