#include "IntegrationTestRunner.h"

#include <cstdlib>
#include <filesystem>

IntegrationTestRunner::IntegrationTestRunner() {}

IntegrationTestRunner::~IntegrationTestRunner() {}

int IntegrationTestRunner::runScript(const std::string& scriptRelativePath)
{
    std::filesystem::path scriptPath;
#ifdef INTEGRATION_TEST_SCRIPTS_DIR
    scriptPath = std::filesystem::path(INTEGRATION_TEST_SCRIPTS_DIR) / scriptRelativePath;
#else
    scriptPath = std::filesystem::current_path() / scriptRelativePath;
#endif

    std::filesystem::path exePath = std::filesystem::current_path() / "CadaverExe.exe";

    std::string command = "\"\"" + exePath.string() + "\" --script \"" + scriptPath.string() + "\"\"";
    return std::system(command.c_str());
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Integration/Python/ClearProject")
{
    CHECK(IntegrationTestRunner::runScript("Python/test_clear_project.py") == 0);
}

TEST_CASE("Integration/Python/GenerateLandscape")
{
    CHECK(IntegrationTestRunner::runScript("Python/test_generate_landscape.py") == 0);
}

TEST_CASE("Integration/Lua/Pipeline")
{
    CHECK(IntegrationTestRunner::runScript("Lua/test_lua_pipeline.lua") == 0);
}
#endif
