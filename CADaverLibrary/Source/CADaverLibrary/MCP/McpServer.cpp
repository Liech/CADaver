#include "MCP/McpServer.h"

#include "Execution/Command.h"
#include "Execution/CommandFactory.h"
#include "Execution/GatedDatabase.h"
#include "Polyglot/Node.h"

#include <iostream>
#include <sstream>

McpServer::McpServer(GatedDatabase& database)
  : gatedDatabase(database)
{
}

McpServer::~McpServer() {}

void McpServer::setCommandExecutor(std::function<nlohmann::json(const nlohmann::json&)> executor)
{
    commandExecutor = std::move(executor);
}

std::string McpServer::handleMessage(const std::string& rawJsonRpc)
{
    nlohmann::json request;
    try
    {
        request = nlohmann::json::parse(rawJsonRpc);
    }
    catch (const std::exception&)
    {
        return createErrorResponse(nullptr, -32700, "Parse error").dump();
    }

    if (!request.is_object())
    {
        return createErrorResponse(nullptr, -32600, "Invalid Request").dump();
    }

    if (!request.contains("method"))
    {
        return "";
    }

    std::string method = request["method"].get<std::string>();

    if (method == "notifications/initialized")
    {
        return "";
    }

    if (!request.contains("id"))
    {
        return "";
    }

    if (method == "initialize")
    {
        return handleInitialize(request).dump();
    }

    if (method == "ping")
    {
        return handlePing(request).dump();
    }

    if (method == "tools/list")
    {
        return handleToolsList(request).dump();
    }

    if (method == "tools/call")
    {
        return handleToolsCall(request).dump();
    }

    return createErrorResponse(request["id"], -32601, "Method not found: " + method).dump();
}

void McpServer::run(std::istream& in, std::ostream& out)
{
    std::string line;
    while (std::getline(in, line))
    {
        if (line.empty())
        {
            continue;
        }

        if (line.back() == '\r')
        {
            line.pop_back();
        }

        std::string response = handleMessage(line);
        if (!response.empty())
        {
            out << response << "\n";
            out.flush();
        }
    }
}

nlohmann::json McpServer::handleInitialize(const nlohmann::json& request)
{
    std::string protocolVersion = "2024-11-05";
    if (request.contains("params") && request["params"].contains("protocolVersion"))
    {
        protocolVersion = request["params"]["protocolVersion"].get<std::string>();
    }

    nlohmann::json result = {
        { "protocolVersion",                                   protocolVersion },
        {    "capabilities",     { { "tools", { { "listChanged", false } } } } },
        {      "serverInfo", { { "name", "cadaver" }, { "version", "1.0.0" } } }
    };
    return createSuccessResponse(request["id"], result);
}

nlohmann::json McpServer::handlePing(const nlohmann::json& request)
{
    return createSuccessResponse(request["id"], nlohmann::json::object());
}

nlohmann::json McpServer::handleToolsList(const nlohmann::json& request)
{
    nlohmann::json tools = nlohmann::json::array();

    tools.push_back({
      {        "name",            "cadaver_execute_command"                      },
      { "description", "Execute any CADaver command by type name and parameters." },
      { "inputSchema",
       { { "type", "object" },
       { "properties",
       { { "type", { { "type", "string" }, { "description", "Command type name (e.g. LoadStlAsset, ClearProject)" } } },
       { "version", { { "type", "integer" }, { "description", "Command version (default 1)" } } } } },
       { "required", { "type" } } }                                              }
    });

    tools.push_back({
      {        "name",                                                  "cadaver_list_commands" },
      { "description", "List all available CADaver commands, their descriptions, and versions." },
      { "inputSchema",     { { "type", "object" }, { "properties", nlohmann::json::object() } } }
    });

    tools.push_back({
      {        "name",                                "cadaver_get_documentation"                      },
      { "description", "Retrieve full documentation and syntax for a command, concept, asset, or note." },
      { "inputSchema",
       { { "type", "object" },
       { "properties", { { "category", { { "type", "string" }, { "enum", { "Command", "Concept", "Asset", "Note" } } } }, { "name", { { "type", "string" }, { "description", "Entity name" } } } } },
       { "required", { "category", "name" } } }                                                        }
    });

    const auto allCommands = gatedDatabase.getCommandFactory().getAllCommands();
    for (const auto& cmd : allCommands)
    {
        if (!cmd)
        {
            continue;
        }

        tools.push_back({
          {        "name",                                                                                         cmd->getType() },
          { "description",                                                                                  cmd->getDescription() },
          { "inputSchema", { { "type", "object" }, { "properties", nlohmann::json::object() }, { "additionalProperties", true } } }
        });
    }

    nlohmann::json result = {
        { "tools", tools }
    };
    return createSuccessResponse(request["id"], result);
}

nlohmann::json McpServer::handleToolsCall(const nlohmann::json& request)
{
    const auto& params   = request.value("params", nlohmann::json::object());
    std::string toolName = params.value("name", "");
    auto        args     = params.value("arguments", nlohmann::json::object());

    if (toolName.empty())
    {
        return createErrorResponse(request["id"], -32602, "Missing tool name in params");
    }

    if (toolName == "cadaver_list_commands")
    {
        nlohmann::json commandsList = nlohmann::json::array();
        const auto     allCommands  = gatedDatabase.getCommandFactory().getAllCommands();
        for (const auto& cmd : allCommands)
        {
            if (cmd)
            {
                commandsList.push_back({
                  {        "type",        cmd->getType() },
                  {     "version",     cmd->getVersion() },
                  { "description", cmd->getDescription() }
                });
            }
        }

        nlohmann::json callResult = {
            { "content", { { { "type", "text" }, { "text", commandsList.dump(2) } } } },
            { "isError",                                                        false }
        };
        return createSuccessResponse(request["id"], callResult);
    }

    nlohmann::json cmdPayload;
    if (toolName == "cadaver_execute_command")
    {
        if (args.contains("command") && args["command"].is_object())
        {
            cmdPayload = args["command"];
        }
        else
        {
            cmdPayload = args;
        }
    }
    else if (toolName == "cadaver_get_documentation")
    {
        cmdPayload = {
            { "type", "GetDocumentation" },
            { "version", 1 },
            { "category", args.value("category", "Command") },
            { "name", args.value("name", "") }
        };
    }
    else
    {
        cmdPayload         = args;
        cmdPayload["type"] = toolName;
    }

    if (!cmdPayload.contains("version"))
    {
        cmdPayload["version"] = 1;
    }

    nlohmann::json executionResult = executeJsonCommand(cmdPayload);

    bool isError = false;
    if (executionResult.value("type", "") == "Error" || executionResult.value("status", "") != "Success")
    {
        isError = true;
    }

    nlohmann::json callResult = {
        { "content", { { { "type", "text" }, { "text", executionResult.dump(2) } } } },
        { "isError",                                                         isError }
    };
    return createSuccessResponse(request["id"], callResult);
}

nlohmann::json McpServer::executeJsonCommand(const nlohmann::json& command)
{
    if (commandExecutor)
    {
        return commandExecutor(command);
    }

    Polyglot::Node inputNode(command);
    Polyglot::Node resultNode = gatedDatabase.execute(inputNode);
    return resultNode.toJson();
}

nlohmann::json McpServer::createErrorResponse(const nlohmann::json& id, int code, const std::string& message) const
{
    return {
        { "jsonrpc",                                        "2.0" },
        {      "id",                                           id },
        {   "error", { { "code", code }, { "message", message } } }
    };
}

nlohmann::json McpServer::createSuccessResponse(const nlohmann::json& id, const nlohmann::json& result) const
{
    return {
        { "jsonrpc",  "2.0" },
        {      "id",     id },
        {  "result", result }
    };
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("McpServer/Initialize")
{
    GatedDatabase database;
    McpServer     server(database);

    std::string request  = R"({"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05"}})";
    std::string response = server.handleMessage(request);

    nlohmann::json json = nlohmann::json::parse(response);
    CHECK(json["jsonrpc"] == "2.0");
    CHECK(json["id"] == 1);
    CHECK(json["result"]["protocolVersion"] == "2024-11-05");
    CHECK(json["result"]["serverInfo"]["name"] == "cadaver");
}

TEST_CASE("McpServer/Ping")
{
    GatedDatabase database;
    McpServer     server(database);

    std::string request  = R"({"jsonrpc":"2.0","id":"req-2","method":"ping"})";
    std::string response = server.handleMessage(request);

    nlohmann::json json = nlohmann::json::parse(response);
    CHECK(json["id"] == "req-2");
    CHECK(json["result"].is_object());
}

TEST_CASE("McpServer/InitializedNotificationReturnsEmpty")
{
    GatedDatabase database;
    McpServer     server(database);

    std::string notification = R"({"jsonrpc":"2.0","method":"notifications/initialized"})";
    std::string response     = server.handleMessage(notification);

    CHECK(response.empty());
}

TEST_CASE("McpServer/ToolsListContainsCommands")
{
    GatedDatabase database;
    McpServer     server(database);

    std::string request  = R"({"jsonrpc":"2.0","id":3,"method":"tools/list"})";
    std::string response = server.handleMessage(request);

    nlohmann::json json = nlohmann::json::parse(response);
    CHECK(json["result"].contains("tools"));
    const auto& tools = json["result"]["tools"];
    CHECK(tools.size() >= 3);

    bool foundClearProject = false;
    for (const auto& tool : tools)
    {
        if (tool["name"] == "ClearProject")
        {
            foundClearProject = true;
            break;
        }
    }
    CHECK(foundClearProject);
}

TEST_CASE("McpServer/ToolsCallClearProject")
{
    GatedDatabase database;
    McpServer     server(database);

    std::string request  = R"({"jsonrpc":"2.0","id":4,"method":"tools/call","params":{"name":"ClearProject","arguments":{}}})";
    std::string response = server.handleMessage(request);

    nlohmann::json json = nlohmann::json::parse(response);
    CHECK(json["id"] == 4);
    CHECK(json["result"]["isError"] == false);
    CHECK(json["result"]["content"].is_array());
    CHECK(json["result"]["content"][0]["type"] == "text");
}

TEST_CASE("McpServer/ToolsCallExecuteCommand")
{
    GatedDatabase database;
    McpServer     server(database);

    std::string request  = R"({"jsonrpc":"2.0","id":5,"method":"tools/call","params":{"name":"cadaver_execute_command","arguments":{"type":"ClearProject"}}})";
    std::string response = server.handleMessage(request);

    nlohmann::json json = nlohmann::json::parse(response);
    CHECK(json["id"] == 5);
    CHECK(json["result"]["isError"] == false);
}

TEST_CASE("McpServer/ToolsCallListCommands")
{
    GatedDatabase database;
    McpServer     server(database);

    std::string request  = R"({"jsonrpc":"2.0","id":6,"method":"tools/call","params":{"name":"cadaver_list_commands","arguments":{}}})";
    std::string response = server.handleMessage(request);

    nlohmann::json json = nlohmann::json::parse(response);
    CHECK(json["id"] == 6);
    CHECK(json["result"]["isError"] == false);
}

TEST_CASE("McpServer/ToolsCallGetDocumentation")
{
    GatedDatabase database;
    McpServer     server(database);

    std::string request  = R"({"jsonrpc":"2.0","id":7,"method":"tools/call","params":{"name":"cadaver_get_documentation","arguments":{"category":"Command","name":"ClearProject"}}})";
    std::string response = server.handleMessage(request);

    nlohmann::json json = nlohmann::json::parse(response);
    CHECK(json["id"] == 7);
    CHECK(json["result"]["isError"] == false);
}

TEST_CASE("McpServer/CustomExecutor")
{
    GatedDatabase database;
    McpServer     server(database);

    bool customExecuted = false;
    server.setCommandExecutor(
      [&customExecuted](const nlohmann::json&) -> nlohmann::json
      {
          customExecuted = true;
          return {
              {   "type", "ClearProject" },
              { "status",      "Success" }
          };
      });

    std::string request  = R"({"jsonrpc":"2.0","id":8,"method":"tools/call","params":{"name":"ClearProject","arguments":{}}})";
    std::string response = server.handleMessage(request);

    nlohmann::json json = nlohmann::json::parse(response);
    CHECK(customExecuted);
    CHECK(json["result"]["isError"] == false);
}

TEST_CASE("McpServer/MethodNotFound")
{
    GatedDatabase database;
    McpServer     server(database);

    std::string request  = R"({"jsonrpc":"2.0","id":9,"method":"unknownMethod"})";
    std::string response = server.handleMessage(request);

    nlohmann::json json = nlohmann::json::parse(response);
    CHECK(json.contains("error"));
    CHECK(json["error"]["code"] == -32601);
}

TEST_CASE("McpServer/StreamRun")
{
    GatedDatabase database;
    McpServer     server(database);

    std::stringstream input;
    input << "{\"jsonrpc\":\"2.0\",\"id\":10,\"method\":\"ping\"}\r\n";
    std::stringstream output;

    server.run(input, output);

    nlohmann::json json = nlohmann::json::parse(output.str());
    CHECK(json["id"] == 10);
    CHECK(json.contains("result"));
}
#endif
