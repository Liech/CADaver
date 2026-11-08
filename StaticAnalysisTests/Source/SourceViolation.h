#pragma once

#include <string>

// Represents a rule violation found during static analysis of source code.
class SourceViolation
{
  public:
    SourceViolation();
    SourceViolation(const std::string& filePath, size_t lineNumber, const std::string& lineContent, const std::string& message);
    virtual ~SourceViolation();

    const std::string& getFilePath() const;
    size_t             getLineNumber() const;
    const std::string& getLineContent() const;
    const std::string& getMessage() const;
    std::string        format() const;

  private:
    std::string filePath;
    std::string lineContent;
    std::string message;
    size_t      lineNumber = 0;
};
