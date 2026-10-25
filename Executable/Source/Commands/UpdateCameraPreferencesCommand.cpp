#include "UpdateCameraPreferencesCommand.h"

#include "Database/Database.h"

UpdateCameraPreferencesCommand::UpdateCameraPreferencesCommand() {}

UpdateCameraPreferencesCommand::UpdateCameraPreferencesCommand(const nlohmann::json& json)
  : preferencesData(json.at("preferences"))
{
}

UpdateCameraPreferencesCommand::~UpdateCameraPreferencesCommand() {}

std::string UpdateCameraPreferencesCommand::getType() const
{
    return "UpdateCameraPreferences";
}

int UpdateCameraPreferencesCommand::getVersion() const
{
    return 1;
}

nlohmann::json UpdateCameraPreferencesCommand::execute(Database& database, std::istream* inputStream, std::ostream* outputStream)
{
    auto& cameraPreferences = database.getPreferences().getCameraPreferences();
    cameraPreferences.fromJson(preferencesData);

    if (preferencesData.contains("clearResetRequested") && preferencesData["clearResetRequested"].get<bool>())
    {
        cameraPreferences.clearResetRequested();
    }
    if (preferencesData.contains("clearModeChangeRequest") && preferencesData["clearModeChangeRequest"].get<bool>())
    {
        cameraPreferences.clearModeChangeRequest();
    }

    return {
        {    "type",    getType() },
        { "version", getVersion() },
        {  "status",    "Success" }
    };
}

const nlohmann::json& UpdateCameraPreferencesCommand::getPreferencesData() const
{
    return preferencesData;
}

void UpdateCameraPreferencesCommand::setPreferencesData(const nlohmann::json& data)
{
    preferencesData = data;
}
