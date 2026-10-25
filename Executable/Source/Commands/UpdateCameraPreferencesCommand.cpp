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

std::string UpdateCameraPreferencesCommand::getDescription() const
{
    return "Command: UpdateCameraPreferences\n"
           "Version: 1\n"
           "Purpose: Updates camera settings (FOV, clip planes, resolution, sensitivity, projection mode).\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"UpdateCameraPreferences\",\n"
           "  \"version\": 1,\n"
           "  \"preferences\": {\n"
           "    \"fov\": <float>,\n"
           "    \"nearClip\": <float>,\n"
           "    \"farClip\": <float>,\n"
           "    \"resolution\": <int>,\n"
           "    \"sensitivity\": <float>,\n"
           "    \"mode\": \"Perspective\" | \"Orthographic\",\n"
           "    \"clearResetRequested\": <bool, optional>,\n"
           "    \"clearModeChangeRequest\": <bool, optional>\n"
           "  }\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: None\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"UpdateCameraPreferences\",\n"
           "  \"version\": 1,\n"
           "  \"status\": \"Success\"\n"
           "}";
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
