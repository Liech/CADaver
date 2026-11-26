#define CATCH_CONFIG_MAIN
#include "StaticAnalysisConfig.h"

#include <catch2/catch_session.hpp>
#include <filesystem>
#include <string>

static void enforceWorkingDir(std::string exeDir)
{
    const size_t lastSlashIdx = exeDir.find_last_of("\\/");
    if (std::string::npos != lastSlashIdx)
    {
        exeDir.erase(lastSlashIdx + 1);
    }
    std::filesystem::current_path(exeDir);
}

int main(int argc, char* argv[])
{
    Catch::Session session;
    std::string    targetDir;

    auto cli = session.cli() | Catch::Clara::Opt(targetDir, "dir")["--target-dir"]("Target directory to scan");
    session.cli(cli);

    int returnCode = session.applyCommandLine(argc, argv);
    if (returnCode != 0)
    {
        return returnCode;
    }

    if (!targetDir.empty())
    {
        StaticAnalysisConfig::setTargetDirectory(std::filesystem::absolute(targetDir).lexically_normal().string());
    }

    enforceWorkingDir(std::string(argv[0]));
    return session.run();
}
