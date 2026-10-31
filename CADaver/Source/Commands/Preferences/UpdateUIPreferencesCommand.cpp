#include "Commands/Preferences/UpdateUIPreferencesCommand.h"

#include "Database/Database.h"

UpdateUIPreferencesCommand::UpdateUIPreferencesCommand() {}

UpdateUIPreferencesCommand::UpdateUIPreferencesCommand(const nlohmann::json& json)
  : preferencesData(json.at("preferences"))
{
}

UpdateUIPreferencesCommand::~UpdateUIPreferencesCommand() {}

std::string UpdateUIPreferencesCommand::getType() const
{
    return "UpdateUIPreferences";
}

int UpdateUIPreferencesCommand::getVersion() const
{
    return 1;
}

std::string UpdateUIPreferencesCommand::getDescription() const
{
    return "Command: UpdateUIPreferences\n"
           "Version: 1\n"
           "Purpose: Updates user interface settings (UI scale, theme index, background color).\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"UpdateUIPreferences\",\n"
           "  \"version\": 1,\n"
           "  \"preferences\": {\n"
           "    \"uiScale\": <float>,\n"
           "    \"themeIndex\": <int>,\n"
           "    \"backgroundColor\": [<float>, <float>, <float>, <float>]\n"
           "  }\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: None\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"UpdateUIPreferences\",\n"
           "  \"version\": 1,\n"
           "  \"status\": \"Success\"\n"
           "}";
}

void UpdateUIPreferencesCommand::setInput(const nlohmann::json& json)
{
    preferencesData = json.at("preferences");
}

nlohmann::json UpdateUIPreferencesCommand::execute(Database& database, std::istream*, std::ostream*)
{
    auto& uiPreferences = database.getPreferences().getUIPreferences();
    previousPreferences = uiPreferences;
    uiPreferences.fromJson(preferencesData);
    newPreferences = uiPreferences;

    return {
        {    "type",    getType() },
        { "version", getVersion() },
        {  "status",    "Success" }
    };
}

bool UpdateUIPreferencesCommand::isUndoable() const
{
    return true;
}

void UpdateUIPreferencesCommand::undo(Database& database)
{
    database.getPreferences().getUIPreferences() = previousPreferences;
}

void UpdateUIPreferencesCommand::redo(Database& database)
{
    database.getPreferences().getUIPreferences() = newPreferences;
}

size_t UpdateUIPreferencesCommand::getUndoSize() const
{
    return sizeof(UpdateUIPreferencesCommand) + preferencesData.dump().capacity();
}

const nlohmann::json& UpdateUIPreferencesCommand::getPreferencesData() const
{
    return preferencesData;
}

void UpdateUIPreferencesCommand::setPreferencesData(const nlohmann::json& data)
{
    preferencesData = data;
}
