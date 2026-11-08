#include "TypeAliasStaticTest.h"

#include "SourceFile.h"
#include "SourceFileDiscoverer.h"

#include <regex>

TypeAliasStaticTest::TypeAliasStaticTest() {}

TypeAliasStaticTest::~TypeAliasStaticTest() {}

std::string TypeAliasStaticTest::stripCommentsAndStrings(const std::string& line) const
{
    static const std::regex commentPattern(R"(//.*$)");
    static const std::regex stringPattern(R"("(?:\\.|[^"\\])*")");
    static const std::regex charPattern(R"('(?:\\.|[^'\\])*')");

    std::string stripped = std::regex_replace(line, commentPattern, "");
    stripped             = std::regex_replace(stripped, stringPattern, "\"\"");
    stripped             = std::regex_replace(stripped, charPattern, "''");
    return stripped;
}

std::vector<SourceViolation> TypeAliasStaticTest::checkContent(const std::string& filePath, const std::string& content) const
{
    std::vector<SourceViolation> violations;
    static const std::regex      usingPattern(R"(^\s*using\s+[a-zA-Z0-9_]+\s*=)");
    static const std::regex      typedefPattern(R"(^\s*typedef\s+)");

    SourceFile  file(filePath, content);
    const auto& lines = file.getLines();
    for (size_t i = 0; i < lines.size(); i++)
    {
        std::string code = stripCommentsAndStrings(lines[i]);
        if (std::regex_search(code, usingPattern) || std::regex_search(code, typedefPattern))
        {
            violations.push_back(SourceViolation(filePath, i + 1, lines[i], "using and typedef type aliases are forbidden"));
        }
    }
    return violations;
}

std::vector<SourceViolation> TypeAliasStaticTest::checkDirectory(const std::string& directoryPath) const
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

TEST_CASE("TypeAliasStaticTest/UnitCheck")
{
    TypeAliasStaticTest checker;
    auto                violationsUsing = checker.checkContent("sample.h", "using MyInt = int;");
    REQUIRE(violationsUsing.size() == 1);
    CHECK(violationsUsing[0].getLineNumber() == 1);

    auto violationsTypedef = checker.checkContent("sample.h", "typedef int OldInt;");
    REQUIRE(violationsTypedef.size() == 1);
    CHECK(violationsTypedef[0].getLineNumber() == 1);

    auto cleanViolations = checker.checkContent("sample.h", "class Foo {};\n// using Bar = Foo;");
    CHECK(cleanViolations.empty());
}

TEST_CASE("TypeAliasStaticTest/ProjectScan")
{
    TypeAliasStaticTest checker;
    auto                violations = checker.checkDirectory(STATIC_ANALYSIS_TARGET_DIR);

    for (const auto& violation : violations)
    {
        UNSCOPED_INFO(violation.format());
    }
    CHECK(violations.empty());
}
#endif
