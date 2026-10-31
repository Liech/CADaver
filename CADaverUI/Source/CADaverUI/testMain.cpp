#define CATCH_CONFIG_MAIN
#include <catch2/catch_session.hpp>

#include <filesystem>

void enforceWorkingDir(std::string exeDir)
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
    enforceWorkingDir(std::string(argv[0]));
    int result = Catch::Session().run(argc, argv);
    return result;
}
