#include "Commands/Preferences/UpdateUIPreferencesCommand.h"

#include "Database/Database.h"
#include "Undo/PreferencesUndoRecord.h"
#include "Undo/UndoStack.h"

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
    auto  prevJson      = uiPreferences.toJson();
    uiPreferences.fromJson(preferencesData);

    UndoStack::getInstance().push(std::make_unique<PreferencesUndoRecord>("UI", prevJson, preferencesData));

    return {
        {    "type",    getType() },
        { "version", getVersion() },
        {  "status",    "Success" }
    };
}

const nlohmann::json& UpdateUIPreferencesCommand::getPreferencesData() const
{
    return preferencesData;
}

void UpdateUIPreferencesCommand::setPreferencesData(const nlohmann::json& data)
{
    preferencesData = data;
}
