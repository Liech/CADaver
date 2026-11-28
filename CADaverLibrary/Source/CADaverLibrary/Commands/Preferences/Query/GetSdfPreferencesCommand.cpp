#include "Commands/Preferences/Query/GetSdfPreferencesCommand.h"

#include "Database/Database.h"

GetSdfPreferencesCommand::GetSdfPreferencesCommand() {}

GetSdfPreferencesCommand::~GetSdfPreferencesCommand() {}

std::string GetSdfPreferencesCommand::getType() const
{
    return "GetSdfPreferences";
}

int GetSdfPreferencesCommand::getVersion() const
{
    return 1;
}

std::string GetSdfPreferencesCommand::getDescription() const
{
    return "Command: GetSdfPreferences\n"
           "Version: 1\n"
           "Purpose: Queries current SDF sphere-tracing and shading preferences.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"GetSdfPreferences\",\n"
           "  \"version\": 1\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: None\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"GetSdfPreferences\",\n"
           "  \"version\": 1,\n"
           "  \"preferences\": <object>\n"
           "}";
}

nlohmann::json GetSdfPreferencesCommand::execute(Database& database)
{
    const auto& sdfPreferences = database.getPreferences().getSdfPreferences();

    return createResponse({
      { "preferences", sdfPreferences.toJson() }
    });
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("GetSdfPreferencesCommand/DefaultPreferences")
{
    Database                 database;
    GetSdfPreferencesCommand cmd;

    nlohmann::json result = cmd.execute(database);
    CHECK(result.at("type") == "GetSdfPreferences");
    CHECK(result.at("version") == 1);
    CHECK(result.at("preferences").at("maxSteps").get<int>() == 128);
}
#endif
