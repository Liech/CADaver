#include "Commands/Preferences/UpdateCameraPreferencesCommand.h"

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
           "Purpose: Updates camera settings (FOV, clip planes, resolution, sensitivity, camera mode).\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"UpdateCameraPreferences\",\n"
           "  \"version\": 1,\n"
           "  \"preferences\": {\n"
           "    \"cameraMode\": <int>,\n"
           "    \"cameraSpeed\": <float>,\n"
           "    \"rotationSensitivity\": <double>,\n"
           "    \"fov\": <float>,\n"
           "    \"nearPlane\": <float>,\n"
           "    \"farPlane\": <float>,\n"
           "    \"resolution\": [<float>, <float>]\n"
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

void UpdateCameraPreferencesCommand::setInput(const nlohmann::json& json)
{
    preferencesData = json.at("preferences");
}

nlohmann::json UpdateCameraPreferencesCommand::execute(Database& database, std::istream*, std::ostream*)
{
    auto& cameraPreferences = database.getPreferences().getCameraPreferences();
    previousPreferences     = cameraPreferences;
    cameraPreferences.fromJson(preferencesData);
    newPreferences = cameraPreferences;

    return {
        {    "type",    getType() },
        { "version", getVersion() },
        {  "status",    "Success" }
    };
}

bool UpdateCameraPreferencesCommand::isUndoable() const
{
    return true;
}

void UpdateCameraPreferencesCommand::undo(Database& database)
{
    database.getPreferences().getCameraPreferences() = previousPreferences;
}

void UpdateCameraPreferencesCommand::redo(Database& database)
{
    database.getPreferences().getCameraPreferences() = newPreferences;
}

size_t UpdateCameraPreferencesCommand::getUndoSize() const
{
    return sizeof(UpdateCameraPreferencesCommand) + preferencesData.dump().capacity();
}

const nlohmann::json& UpdateCameraPreferencesCommand::getPreferencesData() const
{
    return preferencesData;
}

void UpdateCameraPreferencesCommand::setPreferencesData(const nlohmann::json& data)
{
    preferencesData = data;
}
