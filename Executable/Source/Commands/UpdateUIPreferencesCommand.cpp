#include "UpdateUIPreferencesCommand.h"

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
           "Purpose: Updates user interface settings (theme, scaling, layout parameters).\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"UpdateUIPreferences\",\n"
           "  \"version\": 1,\n"
           "  \"preferences\": {\n"
           "    \"theme\": <int>,\n"
           "    \"fontSize\": <float>,\n"
           "    \"uiScale\": <float>\n"
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

nlohmann::json UpdateUIPreferencesCommand::execute(Database& database, std::istream* inputStream, std::ostream* outputStream)
{
    auto& uiPreferences = database.getPreferences().getUIPreferences();
    uiPreferences.fromJson(preferencesData);

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
