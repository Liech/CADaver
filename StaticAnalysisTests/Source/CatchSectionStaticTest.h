#pragma once

#include "SourceViolation.h"

#include <string>
#include <vector>

// Checks that tests contain no Catch2 SECTION blocks, requiring flat independent TEST_CASE blocks.
class CatchSectionStaticTest
{
  public:
    CatchSectionStaticTest();
    virtual ~CatchSectionStaticTest();

    std::vector<SourceViolation> checkDirectory(const std::string& directoryPath) const;
    std::vector<SourceViolation> checkContent(const std::string& filePath, const std::string& content) const;

  private:
    std::string stripCommentsAndStrings(const std::string& line) const;
};
