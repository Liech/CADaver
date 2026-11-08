#include "Commands/Preferences/Query/GetUIPreferencesCommand.h"

#include "Database/Database.h"

GetUIPreferencesCommand::GetUIPreferencesCommand()
{
}

GetUIPreferencesCommand::~GetUIPreferencesCommand()
{
}

std::string GetUIPreferencesCommand::getType() const
{
    return "GetUIPreferences";
}

int GetUIPreferencesCommand::getVersion() const
{
    return 1;
}

std::string GetUIPreferencesCommand::getDescription() const
{
    return "Command: GetUIPreferences\n"
           "Version: 1\n"
           "Purpose: Queries current user interface preferences such as font size and color theme.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"GetUIPreferences\",\n"
           "  \"version\": 1\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: None\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"GetUIPreferences\",\n"
           "  \"version\": 1,\n"
           "  \"status\": \"Success\",\n"
           "  \"preferences\": <object>\n"
           "}";
}

nlohmann::json GetUIPreferencesCommand::execute(Database& database)
{
    const auto& uiPreferences = database.getPreferences().getUIPreferences();

    return createResponse("Success", {
        { "preferences", uiPreferences.toJson() }
    });
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("GetUIPreferencesCommand/DefaultPreferences")
{
    Database                database;
    GetUIPreferencesCommand cmd;

    nlohmann::json result = cmd.execute(database);
    CHECK(result.at("status") == "Success");
    CHECK(result.at("type") == "GetUIPreferences");
    CHECK(result.at("version") == 1);
    CHECK(result.at("preferences").is_object());
    CHECK(result.at("preferences").contains("themeIndex"));
}
#endif
