#include "Commands/Task/GetActiveTasksCommand.h"

#include "Database/Asset/ProgressBarAsset.h"
#include "Database/Database.h"

GetActiveTasksCommand::GetActiveTasksCommand()
{
}

GetActiveTasksCommand::~GetActiveTasksCommand()
{
}

std::string GetActiveTasksCommand::getType() const
{
    return "GetActiveTasks";
}

int GetActiveTasksCommand::getVersion() const
{
    return 1;
}

std::string GetActiveTasksCommand::getDescription() const
{
    return "Command: GetActiveTasks\n"
           "Version: 1\n"
           "Purpose: Queries all active background asynchronous tasks in the project.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"GetActiveTasks\",\n"
           "  \"version\": 1\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: None\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"GetActiveTasks\",\n"
           "  \"version\": 1,\n"
           "  \"tasks\": [\n"
           "    {\n"
           "      \"taskGuid\": <string>,\n"
           "      \"assetGuid\": <string>,\n"
           "      \"operationName\": <string>,\n"
           "      \"progress\": <float>,\n"
           "      \"isReady\": <bool>\n"
           "    }\n"
           "  ]\n"
           "}";
}

nlohmann::json GetActiveTasksCommand::execute(Database& database)
{
    std::vector<nlohmann::json> taskList;
    const auto&                 assets = database.getProject().getAssets();
    for (const auto& asset : assets)
    {
        auto progressAsset = std::dynamic_pointer_cast<ProgressBarAsset>(asset);
        if (progressAsset)
        {
            taskList.push_back({
                {      "taskGuid", progressAsset->getTaskGuid().toString() },
                {     "assetGuid",    progressAsset->getGuid().toString() },
                { "operationName",        progressAsset->getOperationName() },
                {      "progress",            progressAsset->getProgress() },
                {       "isReady",            progressAsset->isReady() }
            });
        }
    }

    return createResponse({
        { "tasks", taskList }
    });
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("GetActiveTasksCommand/NoTasks")
{
    Database              database;
    GetActiveTasksCommand cmd;

    nlohmann::json result = cmd.execute(database);
    CHECK(result.at("type") == "GetActiveTasks");
    CHECK(result.at("version") == 1);
    CHECK(result.at("tasks").empty());
}

TEST_CASE("GetActiveTasksCommand/WithTasks")
{
    Database database;
    auto     task = std::make_shared<ProgressBarAsset>();
    task->setOperationName("LandscapeGeneration");
    task->setTaskGuid(CADacomb::GUID::random());
    database.getProject().addAsset(task);

    GetActiveTasksCommand cmd;
    nlohmann::json        result = cmd.execute(database);
    CHECK(result.at("type") == "GetActiveTasks");
    REQUIRE(result.at("tasks").size() == 1);
    CHECK(result.at("tasks")[0].at("operationName") == "LandscapeGeneration");
}
#endif
