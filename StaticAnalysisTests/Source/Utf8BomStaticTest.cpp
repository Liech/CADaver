#include "Utf8BomStaticTest.h"

#include "SourceFileDiscoverer.h"

#include <fstream>

Utf8BomStaticTest::Utf8BomStaticTest() {}

Utf8BomStaticTest::~Utf8BomStaticTest() {}

bool Utf8BomStaticTest::hasUtf8BomInMemory(const std::vector<uint8_t>& bytes) const
{
    if (bytes.size() < 3)
    {
        return false;
    }
    return (bytes[0] == 0xEF && bytes[1] == 0xBB && bytes[2] == 0xBF);
}

bool Utf8BomStaticTest::hasUtf8Bom(const std::string& filePath) const
{
    std::ifstream stream(filePath, std::ios::binary);
    if (!stream.is_open())
    {
        return false;
    }

    char buffer[3];
    stream.read(buffer, 3);
    if (stream.gcount() < 3)
    {
        return false;
    }

    return (static_cast<unsigned char>(buffer[0]) == 0xEF && static_cast<unsigned char>(buffer[1]) == 0xBB && static_cast<unsigned char>(buffer[2]) == 0xBF);
}

std::vector<SourceViolation> Utf8BomStaticTest::checkDirectory(const std::string& directoryPath) const
{
    std::vector<SourceViolation> violations;
    SourceFileDiscoverer         discoverer;
    auto                         paths = discoverer.discoverFiles(directoryPath, { ".h", ".cpp", ".py", ".lua" });

    for (const auto& path : paths)
    {
        if (!hasUtf8Bom(path))
        {
            violations.push_back(SourceViolation(path, 1, "", "File is missing UTF-8 BOM encoding"));
        }
    }
    return violations;
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

#ifndef STATIC_ANALYSIS_TARGET_DIR
#define STATIC_ANALYSIS_TARGET_DIR "."
#endif

TEST_CASE("Utf8BomStaticTest/UnitCheck")
{
    Utf8BomStaticTest    checker;
    std::vector<uint8_t> validBom = { 0xEF, 0xBB, 0xBF, 0x61, 0x62 };
    CHECK(checker.hasUtf8BomInMemory(validBom) == true);

    std::vector<uint8_t> noBom = { 0x61, 0x62, 0x63 };
    CHECK(checker.hasUtf8BomInMemory(noBom) == false);

    std::vector<uint8_t> tooShort = { 0xEF, 0xBB };
    CHECK(checker.hasUtf8BomInMemory(tooShort) == false);
}

TEST_CASE("Utf8BomStaticTest/ProjectScan")
{
    Utf8BomStaticTest checker;
    auto              violations = checker.checkDirectory(STATIC_ANALYSIS_TARGET_DIR);

    for (const auto& violation : violations)
    {
        UNSCOPED_INFO(violation.format());
    }
    CHECK(violations.empty());
}
#endif
