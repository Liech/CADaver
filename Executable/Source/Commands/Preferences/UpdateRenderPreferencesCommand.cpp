#include "Commands/Preferences/UpdateRenderPreferencesCommand.h"

#include "Commands/Undo/PreferencesUndoRecord.h"
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
           "Purpose: Updates rendering settings (mesh color, wireframe mode, wireframe color, cull mode).\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"UpdateRenderPreferences\",\n"
           "  \"version\": 1,\n"
           "  \"preferences\": {\n"
           "    \"meshColor\": [<float>, <float>, <float>],\n"
           "    \"wireframeMode\": <int>,\n"
           "    \"wireframeColor\": [<float>, <float>, <float>],\n"
           "    \"cullMode\": <int>\n"
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

nlohmann::json UpdateRenderPreferencesCommand::execute(Database& database, std::istream*, std::ostream*, bool recordUndo)
{
    auto& renderPreferences = database.getPreferences().getRenderPreferences();
    auto  prevJson          = renderPreferences.toJson();
    renderPreferences.fromJson(preferencesData);

    if (recordUndo)
    {
        database.getUndoStack().push(std::make_unique<PreferencesUndoRecord>("Render", prevJson, preferencesData));
    }

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
