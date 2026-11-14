#include "Commands/Task/GetTaskStateCommand.h"

#include "Database/Asset/ProgressBarAsset.h"
#include "Database/Database.h"

GetTaskStateCommand::GetTaskStateCommand()
{
}

GetTaskStateCommand::~GetTaskStateCommand()
{
}

std::string GetTaskStateCommand::getType() const
{
    return "GetTaskState";
}

int GetTaskStateCommand::getVersion() const
{
    return 1;
}

std::string GetTaskStateCommand::getDescription() const
{
    return "Command: GetTaskState\n"
           "Version: 1\n"
           "Purpose: Queries the progress and status of a background task by taskGuid or assetGuid.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"GetTaskState\",\n"
           "  \"version\": 1,\n"
           "  \"taskGuid\": <string> (optional),\n"
           "  \"assetGuid\": <string> (optional)\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: None\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"GetTaskState\",\n"
           "  \"version\": 1,\n"
           "  \"taskGuid\": <string>,\n"
           "  \"assetGuid\": <string>,\n"
           "  \"operationName\": <string>,\n"
           "  \"progress\": <float>,\n"
           "  \"isReady\": <bool>\n"
           "}";
}

void GetTaskStateCommand::setInput(const Polyglot::Node& node)
{
    if (node.contains("taskGuid"))
    {
        taskGuid = node["taskGuid"].asString();
    }
    if (node.contains("assetGuid"))
    {
        assetGuid = node["assetGuid"].asString();
    }
}

nlohmann::json GetTaskStateCommand::execute(Database& database)
{
    std::shared_ptr<ProgressBarAsset> foundTask = nullptr;
    const auto&                       assets    = database.getProject().getAssets();

    for (const auto& asset : assets)
    {
        auto progressAsset = std::dynamic_pointer_cast<ProgressBarAsset>(asset);
        if (progressAsset)
        {
            if (!taskGuid.toString().empty() && progressAsset->getTaskGuid() == taskGuid)
            {
                foundTask = progressAsset;
                break;
            }
            if (!assetGuid.toString().empty() && progressAsset->getGuid() == assetGuid)
            {
                foundTask = progressAsset;
                break;
            }
        }
    }

    if (!foundTask)
    {
        return createErrorResponse("TaskNotFound", "Task not found");
    }

    return createResponse({
        {      "taskGuid", foundTask->getTaskGuid().toString() },
        {     "assetGuid",    foundTask->getGuid().toString() },
        { "operationName",        foundTask->getOperationName() },
        {      "progress",            foundTask->getProgress() },
        {       "isReady",            foundTask->isReady() }
    });
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("GetTaskStateCommand/NotFound")
{
    Database            database;
    GetTaskStateCommand cmd;
    cmd.setInput({
        {     "type", "GetTaskState" },
        {  "version",              1 },
        { "taskGuid",      "invalid" }
    });

    nlohmann::json result = cmd.execute(database);
    CHECK(result.at("type") == "Error");
    CHECK(result.at("command") == "GetTaskState");
    CHECK(result.at("errorType") == "TaskNotFound");
}

TEST_CASE("GetTaskStateCommand/FoundByGuid")
{
    Database       database;
    CADacomb::GUID testTaskGuid = CADacomb::GUID::random();
    auto           task         = std::make_shared<ProgressBarAsset>();
    task->setOperationName("VoxelizeModel");
    task->setTaskGuid(testTaskGuid);
    database.getProject().addAsset(task);

    GetTaskStateCommand cmd;
    cmd.setInput({
        {     "type",              "GetTaskState" },
        {  "version",                           1 },
        { "taskGuid", testTaskGuid.toString() }
    });

    nlohmann::json result = cmd.execute(database);
    CHECK(result.at("type") == "GetTaskState");
    CHECK(result.at("operationName") == "VoxelizeModel");
    CHECK(result.at("taskGuid") == testTaskGuid.toString());
}
#endif
