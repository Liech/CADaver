#include "FunctionPointerStaticTest.h"

#include "SourceFile.h"
#include "SourceFileDiscoverer.h"

#include <regex>

FunctionPointerStaticTest::FunctionPointerStaticTest() {}

FunctionPointerStaticTest::~FunctionPointerStaticTest() {}

std::string FunctionPointerStaticTest::stripCommentsAndStrings(const std::string& line) const
{
    static const std::regex commentPattern(R"(//.*$)");
    static const std::regex stringPattern(R"("(?:\\.|[^"\\])*")");
    static const std::regex charPattern(R"('(?:\\.|[^'\\])*')");

    std::string stripped = std::regex_replace(line, commentPattern, "");
    stripped             = std::regex_replace(stripped, stringPattern, "\"\"");
    stripped             = std::regex_replace(stripped, charPattern, "''");
    return stripped;
}

std::vector<SourceViolation> FunctionPointerStaticTest::checkContent(const std::string& filePath, const std::string& content) const
{
    std::vector<SourceViolation> violations;
    static const std::regex      pattern(R"(\b(?:void|int|bool|float|double|char|size_t|uint32_t|int32_t|[A-Za-z0-9_]+)\s*\(\s*\*\s*[A-Za-z0-9_]+\s*\)\s*\()");

    SourceFile  file(filePath, content);
    const auto& lines = file.getLines();
    for (size_t i = 0; i < lines.size(); i++)
    {
        std::string code = stripCommentsAndStrings(lines[i]);
        if (std::regex_search(code, pattern))
        {
            violations.push_back(SourceViolation(filePath, i + 1, lines[i], "C-style function pointer forbidden; use std::function"));
        }
    }
    return violations;
}

std::vector<SourceViolation> FunctionPointerStaticTest::checkDirectory(const std::string& directoryPath) const
{
    std::vector<SourceViolation> violations;
    SourceFileDiscoverer         discoverer;
    auto                         paths = discoverer.discoverFiles(directoryPath, { ".h", ".cpp" });

    for (const auto& path : paths)
    {
        SourceFile file           = SourceFile::loadFromFile(path);
        auto       fileViolations = checkContent(path, file.getContent());
        violations.insert(violations.end(), fileViolations.begin(), fileViolations.end());
    }
    return violations;
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

#ifndef STATIC_ANALYSIS_TARGET_DIR
#define STATIC_ANALYSIS_TARGET_DIR "."
#endif

TEST_CASE("FunctionPointerStaticTest/UnitCheck")
{
    FunctionPointerStaticTest checker;
    auto                      violations = checker.checkContent("sample.cpp", "void (*callback)(int, double);");
    REQUIRE(violations.size() == 1);
    CHECK(violations[0].getLineNumber() == 1);

    auto cleanViolations = checker.checkContent("sample.cpp", "std::function<void(int, double)> callback;");
    CHECK(cleanViolations.empty());
}

TEST_CASE("FunctionPointerStaticTest/ProjectScan")
{
    FunctionPointerStaticTest checker;
    auto                      violations = checker.checkDirectory(STATIC_ANALYSIS_TARGET_DIR);

    for (const auto& violation : violations)
    {
        UNSCOPED_INFO(violation.format());
    }
    CHECK(violations.empty());
}
#endif
