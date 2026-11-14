#pragma once

#include "SourceViolation.h"

#include <string>
#include <vector>

// Checks that source code contains no static_cast to void* or const void*.
class VoidCastStaticTest
{
  public:
    VoidCastStaticTest();
    virtual ~VoidCastStaticTest();

    std::vector<SourceViolation> checkDirectory(const std::string& directoryPath) const;
    std::vector<SourceViolation> checkContent(const std::string& filePath, const std::string& content) const;

  private:
    std::string stripCommentsAndStrings(const std::string& line) const;
};
