#include "Commands/Preferences/Query/GetSVOPreferencesCommand.h"

#include "Database/Database.h"

GetSVOPreferencesCommand::GetSVOPreferencesCommand()
{
}

GetSVOPreferencesCommand::~GetSVOPreferencesCommand()
{
}

std::string GetSVOPreferencesCommand::getType() const
{
    return "GetSVOPreferences";
}

int GetSVOPreferencesCommand::getVersion() const
{
    return 1;
}

std::string GetSVOPreferencesCommand::getDescription() const
{
    return "Command: GetSVOPreferences\n"
           "Version: 1\n"
           "Purpose: Queries current Sparse Voxel Octree (SVO) mesher, LOD, and resolution preferences.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"GetSVOPreferences\",\n"
           "  \"version\": 1\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: None\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"GetSVOPreferences\",\n"
           "  \"version\": 1,\n"
           "  \"status\": \"Success\",\n"
           "  \"preferences\": <object>\n"
           "}";
}

nlohmann::json GetSVOPreferencesCommand::execute(Database& database)
{
    const auto& svoPreferences = database.getPreferences().getSVOPreferences();

    return createResponse("Success", {
        { "preferences", svoPreferences.toJson() }
    });
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("GetSVOPreferencesCommand/DefaultPreferences")
{
    Database                 database;
    GetSVOPreferencesCommand cmd;

    nlohmann::json result = cmd.execute(database);
    CHECK(result.at("status") == "Success");
    CHECK(result.at("type") == "GetSVOPreferences");
    CHECK(result.at("version") == 1);
    CHECK(result.at("preferences").is_object());
    CHECK(result.at("preferences").contains("maxDepth"));
}
#endif
