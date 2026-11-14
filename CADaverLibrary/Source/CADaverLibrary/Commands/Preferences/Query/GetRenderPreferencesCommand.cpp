#include "Commands/Preferences/Query/GetRenderPreferencesCommand.h"

#include "Database/Database.h"

GetRenderPreferencesCommand::GetRenderPreferencesCommand()
{
}

GetRenderPreferencesCommand::~GetRenderPreferencesCommand()
{
}

std::string GetRenderPreferencesCommand::getType() const
{
    return "GetRenderPreferences";
}

int GetRenderPreferencesCommand::getVersion() const
{
    return 1;
}

std::string GetRenderPreferencesCommand::getDescription() const
{
    return "Command: GetRenderPreferences\n"
           "Version: 1\n"
           "Purpose: Queries current mesh color, wireframe styling, and face culling preferences.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"GetRenderPreferences\",\n"
           "  \"version\": 1\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: None\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"GetRenderPreferences\",\n"
           "  \"version\": 1,\n"
           "  \"preferences\": <object>\n"
           "}";
}

nlohmann::json GetRenderPreferencesCommand::execute(Database& database)
{
    const auto& renderPreferences = database.getPreferences().getRenderPreferences();

    return createResponse({
        { "preferences", renderPreferences.toJson() }
    });
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("GetRenderPreferencesCommand/DefaultPreferences")
{
    Database                    database;
    GetRenderPreferencesCommand cmd;

    nlohmann::json result = cmd.execute(database);
    CHECK(result.at("type") == "GetRenderPreferences");
    CHECK(result.at("version") == 1);
    CHECK(result.at("preferences").is_object());
    CHECK(result.at("preferences").contains("wireframeMode"));
}
#endif
