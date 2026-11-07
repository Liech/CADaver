#pragma once

#include "SourceViolation.h"

#include <string>
#include <vector>

// Checks that source code contains no usages of reinterpret_cast.
class ReinterpretCastStaticTest
{
  public:
    ReinterpretCastStaticTest();
    virtual ~ReinterpretCastStaticTest();

    std::vector<SourceViolation> checkDirectory(const std::string& directoryPath) const;
    std::vector<SourceViolation> checkContent(const std::string& filePath, const std::string& content) const;

  private:
    std::string stripCommentsAndStrings(const std::string& line) const;
};
