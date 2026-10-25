#include "UpdateRenderPreferencesCommand.h"

#include "Database/Database.h"

UpdateRenderPreferencesCommand::UpdateRenderPreferencesCommand() {}

UpdateRenderPreferencesCommand::UpdateRenderPreferencesCommand(const nlohmann::json& json)
  : preferencesData(json.at("preferences"))
{
}

UpdateRenderPreferencesCommand::~UpdateRenderPreferencesCommand() {}

std::string UpdateRenderPreferencesCommand::getType() const
{
    return "UpdateRenderPreferences";
}

int UpdateRenderPreferencesCommand::getVersion() const
{
    return 1;
}

std::string UpdateRenderPreferencesCommand::getDescription() const
{
    return "Command: UpdateRenderPreferences\n"
           "Version: 1\n"
           "Purpose: Updates rendering settings (render mode, step size, epsilon, shadows, SSAO, etc.).\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"UpdateRenderPreferences\",\n"
           "  \"version\": 1,\n"
           "  \"preferences\": {\n"
           "    \"renderMode\": <int>,\n"
           "    \"stepSize\": <float>,\n"
           "    \"maxSteps\": <int>,\n"
           "    \"epsilon\": <float>,\n"
           "    \"normalEpsilon\": <float>,\n"
           "    \"ambientOcclusion\": <bool>,\n"
           "    \"shadows\": <bool>,\n"
           "    \"lightDirection\": [<float>, <float>, <float>],\n"
           "    \"lightColor\": [<float>, <float>, <float>],\n"
           "    \"ambientColor\": [<float>, <float>, <float>]\n"
           "  }\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: None\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"UpdateRenderPreferences\",\n"
           "  \"version\": 1,\n"
           "  \"status\": \"Success\"\n"
           "}";
}

nlohmann::json UpdateRenderPreferencesCommand::execute(Database& database, std::istream* inputStream, std::ostream* outputStream)
{
    auto& renderPreferences = database.getPreferences().getRenderPreferences();
    renderPreferences.fromJson(preferencesData);

    return {
        {    "type",    getType() },
        { "version", getVersion() },
        {  "status",    "Success" }
    };
}

const nlohmann::json& UpdateRenderPreferencesCommand::getPreferencesData() const
{
    return preferencesData;
}

void UpdateRenderPreferencesCommand::setPreferencesData(const nlohmann::json& data)
{
    preferencesData = data;
}
