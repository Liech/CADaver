#include "CatchEllipsisStaticTest.h"

#include "SourceFile.h"
#include "SourceFileDiscoverer.h"

#include <regex>

CatchEllipsisStaticTest::CatchEllipsisStaticTest() {}

CatchEllipsisStaticTest::~CatchEllipsisStaticTest() {}

std::string CatchEllipsisStaticTest::stripCommentsAndStrings(const std::string& line) const
{
    static const std::regex commentPattern(R"(//.*$)");
    static const std::regex stringPattern(R"("(?:\\.|[^"\\])*")");
    static const std::regex charPattern(R"('(?:\\.|[^'\\])*')");

    std::string stripped = std::regex_replace(line, commentPattern, "");
    stripped             = std::regex_replace(stripped, stringPattern, "\"\"");
    stripped             = std::regex_replace(stripped, charPattern, "''");
    return stripped;
}

std::vector<SourceViolation> CatchEllipsisStaticTest::checkContent(const std::string& filePath, const std::string& content) const
{
    std::vector<SourceViolation> violations;
    static const std::regex      pattern(R"(\bcatch\s*\(\s*\.\.\.\s*\))");

    SourceFile  file(filePath, content);
    const auto& lines = file.getLines();
    for (size_t i = 0; i < lines.size(); i++)
    {
        std::string code = stripCommentsAndStrings(lines[i]);
        if (std::regex_search(code, pattern))
        {
            violations.push_back(SourceViolation(filePath, i + 1, lines[i], "Use of catch (...) is forbidden"));
        }
    }
    return violations;
}

std::vector<SourceViolation> CatchEllipsisStaticTest::checkDirectory(const std::string& directoryPath) const
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

TEST_CASE("CatchEllipsisStaticTest/UnitCheck")
{
    CatchEllipsisStaticTest checker;
    auto                    violations = checker.checkContent("sample.cpp", "try { foo(); } catch (...) {}");
    REQUIRE(violations.size() == 1);
    CHECK(violations[0].getLineNumber() == 1);

    auto compactViolations = checker.checkContent("sample.cpp", "catch(...) {}");
    REQUIRE(compactViolations.size() == 1);

    auto cleanViolations = checker.checkContent("sample.cpp", "// catch (...) in comment\nstd::string s = \"catch (...)\";\ntry { foo(); } catch (const std::exception& e) {}");
    CHECK(cleanViolations.empty());
}

TEST_CASE("CatchEllipsisStaticTest/ProjectScan")
{
    CatchEllipsisStaticTest checker;
    auto                    violations = checker.checkDirectory(STATIC_ANALYSIS_TARGET_DIR);

    for (const auto& violation : violations)
    {
        UNSCOPED_INFO(violation.format());
    }
    CHECK(violations.empty());
}
#endif
