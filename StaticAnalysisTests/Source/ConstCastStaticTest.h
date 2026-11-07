#pragma once

#include "SourceViolation.h"

#include <string>
#include <vector>

// Checks that source code contains no usages of const_cast.
class ConstCastStaticTest
{
  public:
    ConstCastStaticTest();
    virtual ~ConstCastStaticTest();

    std::vector<SourceViolation> checkDirectory(const std::string& directoryPath) const;
    std::vector<SourceViolation> checkContent(const std::string& filePath, const std::string& content) const;

  private:
    std::string stripCommentsAndStrings(const std::string& line) const;
};
