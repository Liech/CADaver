#include "Commands/Preferences/UpdateSVOPreferencesCommand.h"

#include "Database/Database.h"

UpdateSVOPreferencesCommand::UpdateSVOPreferencesCommand() {}

UpdateSVOPreferencesCommand::~UpdateSVOPreferencesCommand() {}

std::string UpdateSVOPreferencesCommand::getType() const
{
    return "UpdateSVOPreferences";
}

int UpdateSVOPreferencesCommand::getVersion() const
{
    return 1;
}

std::string UpdateSVOPreferencesCommand::getDescription() const
{
    return "Command: UpdateSVOPreferences\n"
           "Version: 1\n"
           "Purpose: Updates SVO LOD visualization settings (strategy, chunk size, vertex capacity, pixel sizes, etc.).\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"UpdateSVOPreferences\",\n"
           "  \"version\": 1,\n"
           "  \"preferences\": {\n"
           "    \"strategyType\": <int>,\n"
           "    \"chunkSize\": <int>,\n"
           "    \"vertexCapacity\": <int>,\n"
           "    \"minPixelSize\": <double>,\n"
           "    \"maxPixelSize\": <double>,\n"
           "    \"targetFillRatio\": <double>,\n"
           "    \"maxDepth\": <int>,\n"
           "    \"frustumCullingEnabled\": <bool>,\n"
           "    \"nearDistance\": <double>,\n"
           "    \"farDistance\": <double>,\n"
           "    \"fixedDecision\": <int>\n"
           "  }\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: None\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"UpdateSVOPreferences\",\n"
           "  \"version\": 1\n"
           "}";
}

void UpdateSVOPreferencesCommand::setInput(const Polyglot::Node& node)
{
    preferencesData = node["preferences"];
}

nlohmann::json UpdateSVOPreferencesCommand::execute(Database& database)
{
    auto& svoPreferences = database.getPreferences().getSVOPreferences();
    previousPreferences  = svoPreferences;
    svoPreferences.fromNode(preferencesData);
    newPreferences = svoPreferences;

    return createResponse();
}

bool UpdateSVOPreferencesCommand::isUndoable() const
{
    return true;
}

void UpdateSVOPreferencesCommand::undo(Database& database)
{
    database.getPreferences().getSVOPreferences() = previousPreferences;
}

void UpdateSVOPreferencesCommand::redo(Database& database)
{
    database.getPreferences().getSVOPreferences() = newPreferences;
}

size_t UpdateSVOPreferencesCommand::getUndoSize() const
{
    return sizeof(UpdateSVOPreferencesCommand);
}
