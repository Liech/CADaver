#include "Commands/Time/GetCurrentTimeCommand.h"

#include "Database/Database.h"

GetCurrentTimeCommand::GetCurrentTimeCommand() {}

GetCurrentTimeCommand::~GetCurrentTimeCommand() {}

std::string GetCurrentTimeCommand::getType() const
{
    return "GetCurrentTime";
}

int GetCurrentTimeCommand::getVersion() const
{
    return 1;
}

std::string GetCurrentTimeCommand::getDescription() const
{
    return "Command: GetCurrentTime\n"
           "Version: 1\n"
           "Purpose: Returns the current project time in seconds.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"GetCurrentTime\",\n"
           "  \"version\": 1\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: None\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"GetCurrentTime\",\n"
           "  \"version\": 1,\n"
           "  \"currentTime\": <double>\n"
           "}";
}

nlohmann::json GetCurrentTimeCommand::execute(Database& database)
{
    return createResponse({
      { "currentTime", database.getCurrentTime() }
    });
}

#ifdef ISTESTPROJECT
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("GetCurrentTimeCommand/ReturnsCurrentTime")
{
    Database              db;
    GetCurrentTimeCommand cmd;
    nlohmann::json        res0 = cmd.execute(db);
    CHECK(res0.at("type").get<std::string>() == "GetCurrentTime");
    CHECK(res0.at("currentTime").get<double>() == Catch::Approx(0.0));

    db.setCurrentTime(2.25);
    nlohmann::json res1 = cmd.execute(db);
    CHECK(res1.at("currentTime").get<double>() == Catch::Approx(2.25));
}
#endif
