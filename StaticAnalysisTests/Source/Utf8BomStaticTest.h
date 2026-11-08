#pragma once

#include "SourceViolation.h"

#include <string>
#include <vector>

// Checks that source code files start with a UTF-8 Byte Order Mark (BOM).
class Utf8BomStaticTest
{
  public:
    Utf8BomStaticTest();
    virtual ~Utf8BomStaticTest();

    std::vector<SourceViolation> checkDirectory(const std::string& directoryPath) const;
    bool                         hasUtf8Bom(const std::string& filePath) const;
    bool                         hasUtf8BomInMemory(const std::vector<uint8_t>& bytes) const;
};
