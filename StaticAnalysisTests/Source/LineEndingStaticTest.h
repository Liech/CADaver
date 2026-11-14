#pragma once

#include "SourceViolation.h"

#include <string>
#include <vector>

// Checks that source files use Unix LF (\n) line endings without CRLF (\r\n).
class LineEndingStaticTest
{
  public:
    LineEndingStaticTest();
    virtual ~LineEndingStaticTest();

    std::vector<SourceViolation> checkDirectory(const std::string& directoryPath) const;
    std::vector<SourceViolation> checkFile(const std::string& filePath) const;
};
