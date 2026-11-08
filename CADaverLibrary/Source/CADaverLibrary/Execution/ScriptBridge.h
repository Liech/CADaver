#pragma once

#include <nlohmann/json.hpp>
#include <string>

namespace Polyglot
{
    class Engine;
    class Node;
}

class GatedDatabase;

// Bridges synchronous database command execution to Polyglot engines.
class ScriptBridge
{
  public:
    ScriptBridge();
    virtual ~ScriptBridge();

    void bind(Polyglot::Engine& engine, GatedDatabase& gatedDatabase);
    void execute(Polyglot::Engine& engine, GatedDatabase& gatedDatabase, const std::string& script);
};
