#include "VoidCastStaticTest.h"

#include "SourceFile.h"
#include "SourceFileDiscoverer.h"

#include <regex>

VoidCastStaticTest::VoidCastStaticTest() {}

VoidCastStaticTest::~VoidCastStaticTest() {}

std::string VoidCastStaticTest::stripCommentsAndStrings(const std::string& line) const
{
    static const std::regex commentPattern(R"(//.*$)");
    static const std::regex stringPattern(R"("(?:\\.|[^"\\])*")");
    static const std::regex charPattern(R"('(?:\\.|[^'\\])*')");

    std::string stripped = std::regex_replace(line, commentPattern, "");
    stripped             = std::regex_replace(stripped, stringPattern, "\"\"");
    stripped             = std::regex_replace(stripped, charPattern, "''");
    return stripped;
}

std::vector<SourceViolation> VoidCastStaticTest::checkContent(const std::string& filePath, const std::string& content) const
{
    std::vector<SourceViolation> violations;
    static const std::regex      pattern(R"(static_cast\s*<\s*(?:const\s+)?void\s*\*\s*>)");

    SourceFile  file(filePath, content);
    const auto& lines = file.getLines();
    for (size_t i = 0; i < lines.size(); i++)
    {
        std::string code = stripCommentsAndStrings(lines[i]);
        if (std::regex_search(code, pattern))
        {
            violations.push_back(SourceViolation(filePath, i + 1, lines[i], "Use of static_cast to void* is forbidden"));
        }
    }
    return violations;
}

std::vector<SourceViolation> VoidCastStaticTest::checkDirectory(const std::string& directoryPath) const
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

TEST_CASE("VoidCastStaticTest/UnitCheck")
{
    VoidCastStaticTest checker;
    auto               violations = checker.checkContent("sample.cpp", "void* p = static_cast<void*>(ptr);\nconst void* cp = static_cast<const void*>(ptr);");
    REQUIRE(violations.size() == 2);
    CHECK(violations[0].getLineNumber() == 1);
    CHECK(violations[1].getLineNumber() == 2);

    auto cleanViolations = checker.checkContent("sample.cpp", "// static_cast<void*>(p)\nstd::string s = \"static_cast<void*>\";");
    CHECK(cleanViolations.empty());
}

TEST_CASE("VoidCastStaticTest/ProjectScan")
{
    VoidCastStaticTest checker;
    auto               violations = checker.checkDirectory(STATIC_ANALYSIS_TARGET_DIR);

    for (const auto& violation : violations)
    {
        UNSCOPED_INFO(violation.format());
    }
    CHECK(violations.empty());
}
#endif
