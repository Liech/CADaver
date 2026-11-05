#include "Execution/ScriptBridge.h"

#include "Execution/GatedDatabase.h"
#include "Polyglot/Engine.h"
#include "Polyglot/Node.h"

#include <functional>

ScriptBridge::ScriptBridge() {}

ScriptBridge::~ScriptBridge() {}

void ScriptBridge::bind(Polyglot::Engine& engine, GatedDatabase& gatedDatabase)
{
    std::function<Polyglot::Node(const Polyglot::Node&)> cadaverCallback = [this, &gatedDatabase](const Polyglot::Node& input) -> Polyglot::Node
    {
        Polyglot::Node result = gatedDatabase.execute(input);
        if (static_cast<std::string>(result["type"]) == "Error")
        {
            throw std::runtime_error(static_cast<std::string>(result["error"]));
        }

        return result;
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
    engine.executeString("result = cadaver({type = \"ClearProject\", version = 1})");
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
    engine.executeString("result = cadaver({\"type\": \"ClearProject\", \"version\": 1})");
    Polyglot::Node result = engine.getVar("result");
    nlohmann::json json   = result.toJson();
    CHECK(json["status"] == "Success");
}

TEST_CASE("ScriptBridge/ExecuteConvenience")
{
    GatedDatabase            gatedDatabase;
    Polyglot::Lua::LuaEngine engine;
    ScriptBridge             bridge;

    bridge.execute(engine, gatedDatabase, "result = cadaver({type = \"ClearProject\", version = 1})");
    Polyglot::Node result = engine.getVar("result");
    nlohmann::json json   = result.toJson();
    CHECK(json["status"] == "Success");
}

TEST_CASE("ScriptBridge/ExecuteErrorThrowsLua")
{
    GatedDatabase            gatedDatabase;
    Polyglot::Lua::LuaEngine engine;
    ScriptBridge             bridge;

    bridge.bind(engine, gatedDatabase);
    CHECK_THROWS_AS(engine.executeString("cadaver({type = \"NonExistentCommand\", version = 1})"), std::runtime_error);
}

TEST_CASE("ScriptBridge/ExecuteErrorThrowsPython")
{
    GatedDatabase                  gatedDatabase;
    Polyglot::Python::PythonEngine engine;
    ScriptBridge                   bridge;

    bridge.bind(engine, gatedDatabase);
    CHECK_THROWS(engine.executeString("cadaver({\"type\": \"NonExistentCommand\", \"version\": 1})"));
}
#endif
