#include "SourceViolation.h"

SourceViolation::SourceViolation() {}

SourceViolation::SourceViolation(const std::string& filePathInput, size_t lineNumberInput, const std::string& lineContentInput, const std::string& messageInput)
{
    filePath    = filePathInput;
    lineNumber  = lineNumberInput;
    lineContent = lineContentInput;
    message     = messageInput;
}

SourceViolation::~SourceViolation() {}

const std::string& SourceViolation::getFilePath() const
{
    return filePath;
}

size_t SourceViolation::getLineNumber() const
{
    return lineNumber;
}

const std::string& SourceViolation::getLineContent() const
{
    return lineContent;
}

const std::string& SourceViolation::getMessage() const
{
    return message;
}

std::string SourceViolation::format() const
{
    return filePath + ":" + std::to_string(lineNumber) + ": " + message + " -> " + lineContent;
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("SourceViolation/Format")
{
    SourceViolation violation("Foo.cpp", 42, "const_cast<int*>(x)", "Forbidden cast");
    CHECK(violation.getFilePath() == "Foo.cpp");
    CHECK(violation.getLineNumber() == 42);
    CHECK(violation.getLineContent() == "const_cast<int*>(x)");
    CHECK(violation.getMessage() == "Forbidden cast");
    CHECK(violation.format() == "Foo.cpp:42: Forbidden cast -> const_cast<int*>(x)");
}
#endif
