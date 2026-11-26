#include "StaticAnalysisConfig.h"

#ifndef STATIC_ANALYSIS_TARGET_DIR
#define STATIC_ANALYSIS_TARGET_DIR "."
#endif

std::string StaticAnalysisConfig::targetDirectory = STATIC_ANALYSIS_TARGET_DIR;

StaticAnalysisConfig::StaticAnalysisConfig() {}

StaticAnalysisConfig::~StaticAnalysisConfig() {}

std::string StaticAnalysisConfig::getTargetDirectory()
{
    return targetDirectory;
}

void StaticAnalysisConfig::setTargetDirectory(const std::string& directory)
{
    targetDirectory = directory;
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("StaticAnalysisConfig/SetAndGetTargetDirectory")
{
    std::string previous = StaticAnalysisConfig::getTargetDirectory();
    StaticAnalysisConfig::setTargetDirectory("custom/path");
    CHECK(StaticAnalysisConfig::getTargetDirectory() == "custom/path");
    StaticAnalysisConfig::setTargetDirectory(previous);
    CHECK(StaticAnalysisConfig::getTargetDirectory() == previous);
}
#endif
