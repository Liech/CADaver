#include "Commands/Preferences/UpdateSdfPreferencesCommand.h"

#include "Database/Database.h"

UpdateSdfPreferencesCommand::UpdateSdfPreferencesCommand() {}

UpdateSdfPreferencesCommand::~UpdateSdfPreferencesCommand() {}

std::string UpdateSdfPreferencesCommand::getType() const
{
    return "UpdateSdfPreferences";
}

int UpdateSdfPreferencesCommand::getVersion() const
{
    return 1;
}

std::string UpdateSdfPreferencesCommand::getDescription() const
{
    return "Command: UpdateSdfPreferences\n"
           "Version: 1\n"
           "Purpose: Updates SDF sphere-tracing and shading preferences.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"UpdateSdfPreferences\",\n"
           "  \"version\": 1,\n"
           "  \"preferences\": {\n"
           "    \"Version\": 1,\n"
           "    \"maxSteps\": <int>,\n"
           "    \"hitEpsilon\": <double>,\n"
           "    \"stepScale\": <double>,\n"
           "    \"normalEpsilon\": <double>,\n"
           "    \"ambientStrength\": <double>\n"
           "  }\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: None\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"UpdateSdfPreferences\",\n"
           "  \"version\": 1\n"
           "}";
}

void UpdateSdfPreferencesCommand::setInput(const Polyglot::Node& node)
{
    preferencesData = node["preferences"];
}

nlohmann::json UpdateSdfPreferencesCommand::execute(Database& database)
{
    auto& sdfPreferences = database.getPreferences().getSdfPreferences();
    previousPreferences  = sdfPreferences;
    sdfPreferences.fromNode(preferencesData);
    newPreferences = sdfPreferences;

    return createResponse();
}

bool UpdateSdfPreferencesCommand::isUndoable() const
{
    return true;
}

void UpdateSdfPreferencesCommand::undo(Database& database)
{
    database.getPreferences().getSdfPreferences() = previousPreferences;
}

void UpdateSdfPreferencesCommand::redo(Database& database)
{
    database.getPreferences().getSdfPreferences() = newPreferences;
}

size_t UpdateSdfPreferencesCommand::getUndoSize() const
{
    return sizeof(UpdateSdfPreferencesCommand);
}
