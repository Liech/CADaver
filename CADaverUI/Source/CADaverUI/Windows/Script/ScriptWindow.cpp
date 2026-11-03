#include "Windows/Script/ScriptWindow.h"

#include "Execution/GatedDatabase.h"
#include "Execution/ScriptBridge.h"
#include "Polyglot/Lua/LuaEngine.h"
#include "Polyglot/Python/PythonEngine.h"

#include <cstring>
#include <imgui.h>

ScriptWindow::ScriptWindow()
{
    scriptBuffer.resize(65536, '\0');
}

ScriptWindow::~ScriptWindow() {}

std::string ScriptWindow::getType() const
{
    return "Script";
}

std::string ScriptWindow::getDescription() const
{
    return "Window: Script\n"
           "Title: Script Runner\n"
           "Purpose: Executes Python or Lua scripts against the database using Polyglot engines and the cadaver(input) command bridge.\n\n"
           "Parameters:\n"
           "  - Language: ComboBox selecting Python or Lua\n"
           "  - Script: Multi-line text editor for script input\n"
           "  - Run: Button executing the script synchronously\n\n"
           "Dispatched Command: Dynamic based on cadaver() invocations inside the script";
}

void ScriptWindow::setScript(const std::string& script)
{
    scriptText = script;
    if (scriptBuffer.size() <= script.size())
    {
        scriptBuffer.resize(script.size() + 1024, '\0');
    }

    std::memcpy(scriptBuffer.data(), script.data(), script.size());
    scriptBuffer[script.size()] = '\0';
}

const std::string& ScriptWindow::getScript() const
{
    return scriptText;
}

void ScriptWindow::setLanguage(int index)
{
    selectedLanguage = index;
}

int ScriptWindow::getLanguage() const
{
    return selectedLanguage;
}

const std::string& ScriptWindow::getOutput() const
{
    return outputMessage;
}

bool ScriptWindow::hasExecutionError() const
{
    return hasError;
}

void ScriptWindow::runScript()
{
    scriptText = std::string(scriptBuffer.data());

    if (!gatedDatabase)
    {
        outputMessage = "Error: Database not available.";
        hasError      = true;
        return;
    }

    try
    {
        ScriptBridge bridge;
        if (selectedLanguage == 0)
        {
            Polyglot::Python::PythonEngine engine;
            bridge.execute(engine, *gatedDatabase, scriptText);
        }
        else
        {
            Polyglot::Lua::LuaEngine engine;
            bridge.execute(engine, *gatedDatabase, scriptText);
        }

        outputMessage = "Execution completed successfully.";
        hasError      = false;
    }
    catch (const std::exception& exception)
    {
        outputMessage = std::string("Error: ") + exception.what();
        hasError      = true;
    }
}

void ScriptWindow::renderControls()
{
    const char* languages = "Python\0Lua\0\0";
    ImGui::Combo("Language", &selectedLanguage, languages);

    ImGui::SameLine();
    if (ImGui::Button("Run", ImVec2(100.0f, 0.0f)))
    {
        runScript();
    }
}

void ScriptWindow::renderEditor()
{
    float reservedHeight = 80.0f;
    ImGui::InputTextMultiline("##ScriptCode", scriptBuffer.data(), scriptBuffer.size(), ImVec2(-1.0f, -reservedHeight));
    scriptText = std::string(scriptBuffer.data());
}

void ScriptWindow::renderOutput()
{
    if (outputMessage.empty())
    {
        return;
    }

    ImGui::Separator();
    if (hasError)
    {
        ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "%s", outputMessage.c_str());
    }
    else
    {
        ImGui::TextColored(ImVec4(0.3f, 1.0f, 0.3f, 1.0f), "%s", outputMessage.c_str());
    }
}

void ScriptWindow::render()
{
    if (!openState)
    {
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(600.0f, 400.0f), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Script Runner", &openState))
    {
        handleEscapeClose();
        renderControls();
        renderEditor();
        renderOutput();
    }
    ImGui::End();
}

#ifdef ISTESTPROJECT
#include "Execution/Defer.h"
#include "Windows/Windows.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("ScriptWindow/DefaultState")
{
    ScriptWindow window;
    CHECK(window.getType() == "Script");
    CHECK(window.getLanguage() == 0);
    CHECK(window.getScript().empty());
    CHECK(window.getOutput().empty());
    CHECK(!window.hasExecutionError());
}

TEST_CASE("ScriptWindow/SetScriptAndLanguage")
{
    ScriptWindow window;
    window.setLanguage(1);
    CHECK(window.getLanguage() == 1);

    std::string testScript = "cadaver({type = \"ClearProject\"})";
    window.setScript(testScript);
    CHECK(window.getScript() == testScript);
}

TEST_CASE("ScriptWindow/RunWithoutDatabase")
{
    ScriptWindow window;
    window.setScript("test");
    window.runScript();
    CHECK(window.hasExecutionError());
    CHECK(window.getOutput() == "Error: Database not available.");
}

TEST_CASE("ScriptWindow/RunWithDatabaseLua")
{
    GatedDatabase gatedDatabase;
    Defer         defer;
    Windows       windows;

    ScriptWindow window;
    window.setInput(gatedDatabase, defer, windows);
    window.setLanguage(1);
    window.setScript("cadaver({type = \"ClearProject\"})");
    window.runScript();

    CHECK(!window.hasExecutionError());
    CHECK(window.getOutput() == "Execution completed successfully.");
}

TEST_CASE("ScriptWindow/RunWithDatabasePython")
{
    GatedDatabase gatedDatabase;
    Defer         defer;
    Windows       windows;

    ScriptWindow window;
    window.setInput(gatedDatabase, defer, windows);
    window.setLanguage(0);
    window.setScript("cadaver({\"type\": \"ClearProject\"})");
    window.runScript();

    CHECK(!window.hasExecutionError());
    CHECK(window.getOutput() == "Execution completed successfully.");
}
#endif
