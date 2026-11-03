#include "HeadlessScriptRunner.h"

#include "CommandLineArgs.h"
#include "Execution/GatedDatabase.h"
#include "Execution/ScriptBridge.h"
#include "Polyglot/Lua/LuaEngine.h"
#include "Polyglot/Python/PythonEngine.h"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>

HeadlessScriptRunner::HeadlessScriptRunner() {}

HeadlessScriptRunner::~HeadlessScriptRunner() {}

int HeadlessScriptRunner::run(const CommandLineArgs& args)
{
    std::string scriptPath = args.getScriptPath();
    std::string code       = readScriptFile(scriptPath);

    if (code.empty())
    {
        std::cerr << "Error: Failed to read script file: " << scriptPath << std::endl;
        writeOutputFile(args.getOutputPath(), false, "Failed to read script file: " + scriptPath);
        return EXIT_FAILURE;
    }

    GatedDatabase gatedDatabase;
    if (args.isVerbose())
    {
        gatedDatabase.setLogCallback([](const std::string& msg) { std::cerr << msg << std::endl; });
    }
    else
    {
        gatedDatabase.setLogCallback([](const std::string&) {});
    }

    std::string lang = determineLanguage(args);

    try
    {
        ScriptBridge bridge;
        if (lang == "python")
        {
            Polyglot::Python::PythonEngine engine;
            bridge.execute(engine, gatedDatabase, code);
        }
        else
        {
            Polyglot::Lua::LuaEngine engine;
            bridge.execute(engine, gatedDatabase, code);
        }

        writeOutputFile(args.getOutputPath(), true, "");
        return EXIT_SUCCESS;
    }
    catch (const std::exception& exception)
    {
        std::cerr << "Script Error: " << exception.what() << std::endl;
        writeOutputFile(args.getOutputPath(), false, exception.what());
        return EXIT_FAILURE;
    }
}

std::string HeadlessScriptRunner::readScriptFile(const std::string& path) const
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::string HeadlessScriptRunner::determineLanguage(const CommandLineArgs& args) const
{
    if (!args.getLanguage().empty())
    {
        std::string lang = args.getLanguage();
        if (lang == "py" || lang == "python" || lang == "Python")
        {
            return "python";
        }

        return "lua";
    }

    const std::string& path = args.getScriptPath();
    if (path.size() >= 3 && path.substr(path.size() - 3) == ".py")
    {
        return "python";
    }

    return "lua";
}

void HeadlessScriptRunner::writeOutputFile(const std::string& path, bool success, const std::string& errorMessage) const
{
    if (path.empty())
    {
        return;
    }

    std::ofstream file(path);
    if (!file.is_open())
    {
        return;
    }

    nlohmann::json output = {
        { "success",      success },
        {   "error", errorMessage }
    };

    file << output.dump(4);
}
