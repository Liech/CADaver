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
