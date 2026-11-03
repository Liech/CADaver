#include "Execution/ScriptBridge.h"

#include "Execution/GatedDatabase.h"
#include "Polyglot/Engine.h"
#include "Polyglot/Node.h"

#include <functional>

ScriptBridge::ScriptBridge() {}

ScriptBridge::~ScriptBridge() {}

nlohmann::json ScriptBridge::convertNodeToJson(const Polyglot::Node& node) const
{
    nlohmann::json commandJson;
    if (std::holds_alternative<std::string>(node.value))
    {
        commandJson = nlohmann::json::parse(std::get<std::string>(node.value));
    }
    else
    {
        commandJson = node.toJson();
    }

    if (commandJson.is_object() && !commandJson.contains("version"))
    {
        commandJson["version"] = 1;
    }

    return commandJson;
}

void ScriptBridge::bind(Polyglot::Engine& engine, GatedDatabase& gatedDatabase)
{
    std::function<Polyglot::Node(const Polyglot::Node&)> cadaverCallback = [this, &gatedDatabase](const Polyglot::Node& input) -> Polyglot::Node
    {
        nlohmann::json commandJson = convertNodeToJson(input);
        nlohmann::json result      = gatedDatabase.execute(commandJson);
        return Polyglot::Node(result);
    };

    engine.setVar("cadaver", cadaverCallback);
}

void ScriptBridge::execute(Polyglot::Engine& engine, GatedDatabase& gatedDatabase, const std::string& script)
{
    bind(engine, gatedDatabase);
    engine.executeString(script);
}

#ifdef ISTESTPROJECT
#include "Polyglot/Lua/LuaEngine.h"
#include "Polyglot/Python/PythonEngine.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("ScriptBridge/BindAndExecuteLua")
{
    GatedDatabase            gatedDatabase;
    Polyglot::Lua::LuaEngine engine;
    ScriptBridge             bridge;

    bridge.bind(engine, gatedDatabase);
    engine.executeString("result = cadaver({type = \"ClearProject\"})");
    Polyglot::Node result = engine.getVar("result");
    nlohmann::json json   = result.toJson();
    CHECK(json["status"] == "Success");
}

TEST_CASE("ScriptBridge/BindAndExecutePython")
{
    GatedDatabase                  gatedDatabase;
    Polyglot::Python::PythonEngine engine;
    ScriptBridge                   bridge;

    bridge.bind(engine, gatedDatabase);
    engine.executeString("result = cadaver({\"type\": \"ClearProject\"})");
    Polyglot::Node result = engine.getVar("result");
    nlohmann::json json   = result.toJson();
    CHECK(json["status"] == "Success");
}

TEST_CASE("ScriptBridge/ExecuteConvenience")
{
    GatedDatabase            gatedDatabase;
    Polyglot::Lua::LuaEngine engine;
    ScriptBridge             bridge;

    bridge.execute(engine, gatedDatabase, "result = cadaver({type = \"ClearProject\"})");
    Polyglot::Node result = engine.getVar("result");
    nlohmann::json json   = result.toJson();
    CHECK(json["status"] == "Success");
}
#endif
