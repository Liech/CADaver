#include "LineEndingStaticTest.h"

#include "SourceFileDiscoverer.h"

#include <fstream>

LineEndingStaticTest::LineEndingStaticTest() {}

LineEndingStaticTest::~LineEndingStaticTest() {}

std::vector<SourceViolation> LineEndingStaticTest::checkFile(const std::string& filePath) const
{
    std::vector<SourceViolation> violations;
    std::ifstream                stream(filePath, std::ios::binary);
    if (!stream.is_open())
    {
        return violations;
    }

    std::string content((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());

    size_t line = 1;
    for (size_t i = 0; i + 1 < content.size(); i++)
    {
        if (content[i] == '\r')
        {
            if (content[i + 1] == '\n')
            {
                violations.push_back(SourceViolation(filePath, line, "\\r\\n", "CRLF line ending forbidden; use LF"));
                break;
            }
        }
        else if (content[i] == '\n')
        {
            line++;
        }
    }
    return violations;
}

std::vector<SourceViolation> LineEndingStaticTest::checkDirectory(const std::string& directoryPath) const
{
    std::vector<SourceViolation> violations;
    SourceFileDiscoverer         discoverer;
    auto                         paths = discoverer.discoverFiles(directoryPath, { ".h", ".cpp", ".py", ".lua" });

    for (const auto& path : paths)
    {
        auto fileViolations = checkFile(path);
        violations.insert(violations.end(), fileViolations.begin(), fileViolations.end());
    }
    return violations;
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

#ifndef STATIC_ANALYSIS_TARGET_DIR
#define STATIC_ANALYSIS_TARGET_DIR "."
#endif

TEST_CASE("LineEndingStaticTest/UnitCheck")
{
    LineEndingStaticTest checker;
    // Unit check uses checkDirectory or file mock
    CHECK(checker.checkFile("nonexistent.cpp").empty());
}

TEST_CASE("LineEndingStaticTest/ProjectScan")
{
    LineEndingStaticTest checker;
    auto                 violations = checker.checkDirectory(STATIC_ANALYSIS_TARGET_DIR);

    for (const auto& violation : violations)
    {
        UNSCOPED_INFO(violation.format());
    }
    CHECK(violations.empty());
}
#endif
