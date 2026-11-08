#include "DefaultKeywordStaticTest.h"

#include "SourceFile.h"
#include "SourceFileDiscoverer.h"

#include <regex>

DefaultKeywordStaticTest::DefaultKeywordStaticTest() {}

DefaultKeywordStaticTest::~DefaultKeywordStaticTest() {}

std::string DefaultKeywordStaticTest::stripCommentsAndStrings(const std::string& line) const
{
    static const std::regex commentPattern(R"(//.*$)");
    static const std::regex stringPattern(R"("(?:\\.|[^"\\])*")");
    static const std::regex charPattern(R"('(?:\\.|[^'\\])*')");

    std::string stripped = std::regex_replace(line, commentPattern, "");
    stripped             = std::regex_replace(stripped, stringPattern, "\"\"");
    stripped             = std::regex_replace(stripped, charPattern, "''");
    return stripped;
}

std::vector<SourceViolation> DefaultKeywordStaticTest::checkContent(const std::string& filePath, const std::string& content) const
{
    std::vector<SourceViolation> violations;
    static const std::regex      pattern(R"(=\s*default\s*;)");

    SourceFile  file(filePath, content);
    const auto& lines = file.getLines();
    for (size_t i = 0; i < lines.size(); i++)
    {
        std::string code = stripCommentsAndStrings(lines[i]);
        if (std::regex_search(code, pattern))
        {
            violations.push_back(SourceViolation(filePath, i + 1, lines[i], "= default is forbidden; write explicit bodies in .cpp"));
        }
    }
    return violations;
}

std::vector<SourceViolation> DefaultKeywordStaticTest::checkDirectory(const std::string& directoryPath) const
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

TEST_CASE("DefaultKeywordStaticTest/UnitCheck")
{
    DefaultKeywordStaticTest checker;
    auto                     violations = checker.checkContent("sample.h", "Foo() = default;");
    REQUIRE(violations.size() == 1);
    CHECK(violations[0].getLineNumber() == 1);

    auto cleanViolations = checker.checkContent("sample.h", "Foo();\n// Foo() = default;\nstd::string s = \"= default;\";");
    CHECK(cleanViolations.empty());
}

TEST_CASE("DefaultKeywordStaticTest/ProjectScan")
{
    DefaultKeywordStaticTest checker;
    auto                     violations = checker.checkDirectory(STATIC_ANALYSIS_TARGET_DIR);

    for (const auto& violation : violations)
    {
        UNSCOPED_INFO(violation.format());
    }
    CHECK(violations.empty());
}
#endif
