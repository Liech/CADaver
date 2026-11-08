#include "Commands/Preferences/Query/GetCameraPreferencesCommand.h"

#include "Database/Database.h"

GetCameraPreferencesCommand::GetCameraPreferencesCommand()
{
}

GetCameraPreferencesCommand::~GetCameraPreferencesCommand()
{
}

std::string GetCameraPreferencesCommand::getType() const
{
    return "GetCameraPreferences";
}

int GetCameraPreferencesCommand::getVersion() const
{
    return 1;
}

std::string GetCameraPreferencesCommand::getDescription() const
{
    return "Command: GetCameraPreferences\n"
           "Version: 1\n"
           "Purpose: Queries current camera controls, sensitivity, and frustum clipping preferences.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"GetCameraPreferences\",\n"
           "  \"version\": 1\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: None\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"GetCameraPreferences\",\n"
           "  \"version\": 1,\n"
           "  \"status\": \"Success\",\n"
           "  \"preferences\": <object>\n"
           "}";
}

nlohmann::json GetCameraPreferencesCommand::execute(Database& database)
{
    const auto& cameraPreferences = database.getPreferences().getCameraPreferences();

    return createResponse("Success", {
        { "preferences", cameraPreferences.toJson() }
    });
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("GetCameraPreferencesCommand/DefaultPreferences")
{
    Database                    database;
    GetCameraPreferencesCommand cmd;

    nlohmann::json result = cmd.execute(database);
    CHECK(result.at("status") == "Success");
    CHECK(result.at("type") == "GetCameraPreferences");
    CHECK(result.at("version") == 1);
    CHECK(result.at("preferences").is_object());
    CHECK(result.at("preferences").contains("cameraSpeed"));
}
#endif
