#include "Commands/Project/ClearProjectCommand.h"

#include "Database/Asset.h"
#include "Database/Database.h"

ClearProjectCommand::ClearProjectCommand() {}

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

nlohmann::json ClearProjectCommand::execute(Database& database, std::istream*, std::ostream*)
{
    clearedAssets = database.getProject().getAssets();

    database.getProject().clear();

    return {
        {    "type",    getType() },
        { "version", getVersion() },
        {  "status",    "Success" }
    };
}

bool ClearProjectCommand::isUndoable() const
{
    return !clearedAssets.empty();
}

void ClearProjectCommand::undo(Database& database)
{
    auto& project = database.getProject();
    project.clear();
    for (const auto& asset : clearedAssets)
    {
        project.addAsset(asset);
    }
}

void ClearProjectCommand::redo(Database& database)
{
    database.getProject().clear();
}

size_t ClearProjectCommand::getUndoSize() const
{
    size_t size = sizeof(ClearProjectCommand);
    for (const auto& asset : clearedAssets)
    {
        if (asset)
        {
            size += asset->getSizeInBytes();
        }
    }
    return size;
}
