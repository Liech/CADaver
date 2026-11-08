#include "Commands/Task/CancelTaskCommand.h"

#include "Database/Asset/ProgressBarAsset.h"
#include "Database/Database.h"

CancelTaskCommand::CancelTaskCommand()
{
}

CancelTaskCommand::~CancelTaskCommand()
{
}

std::string CancelTaskCommand::getType() const
{
    return "CancelTask";
}

int CancelTaskCommand::getVersion() const
{
    return 1;
}

std::string CancelTaskCommand::getDescription() const
{
    return "Command: CancelTask\n"
           "Version: 1\n"
           "Purpose: Requests cancellation of an in-progress background task.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"CancelTask\",\n"
           "  \"version\": 1,\n"
           "  \"taskGuid\": <string> (optional),\n"
           "  \"assetGuid\": <string> (optional)\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: None\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"CancelTask\",\n"
           "  \"version\": 1,\n"
           "  \"status\": \"Success\" | \"TaskNotFound\",\n"
           "  \"taskGuid\": <string>\n"
           "}";
}

void CancelTaskCommand::setInput(const Polyglot::Node& node)
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

nlohmann::json CancelTaskCommand::execute(Database& database)
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
        return createResponse("TaskNotFound", {
            { "taskGuid", taskGuid.toString() }
        });
    }

    foundTask->cancel();

    return createResponse("Success", {
        { "taskGuid", foundTask->getTaskGuid().toString() }
    });
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("CancelTaskCommand/NotFound")
{
    Database          database;
    CancelTaskCommand cmd;
    cmd.setInput({
        {     "type", "CancelTask" },
        {  "version",            1 },
        { "taskGuid",    "invalid" }
    });

    nlohmann::json result = cmd.execute(database);
    CHECK(result.at("status") == "TaskNotFound");
}

TEST_CASE("CancelTaskCommand/CancelSuccess")
{
    Database       database;
    CADacomb::GUID testTaskGuid = CADacomb::GUID::random();
    auto           task         = std::make_shared<ProgressBarAsset>();
    task->setOperationName("ConvertCadToSvo");
    task->setTaskGuid(testTaskGuid);
    database.getProject().addAsset(task);

    CancelTaskCommand cmd;
    cmd.setInput({
        {     "type",         "CancelTask" },
        {  "version",                    1 },
        { "taskGuid", testTaskGuid.toString() }
    });

    nlohmann::json result = cmd.execute(database);
    CHECK(result.at("status") == "Success");
    CHECK(result.at("taskGuid") == testTaskGuid.toString());
}
#endif
