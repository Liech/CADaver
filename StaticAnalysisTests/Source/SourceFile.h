#pragma once

#include <string>
#include <vector>

// Encapsulates a source code file with its path and contents for static analysis.
class SourceFile
{
  public:
    SourceFile();
    SourceFile(const std::string& filePath, const std::string& content);
    virtual ~SourceFile();

    const std::string&              getFilePath() const;
    const std::string&              getContent() const;
    const std::vector<std::string>& getLines() const;

    static SourceFile loadFromFile(const std::string& absolutePath);

  private:
    std::string              filePath;
    std::string              content;
    std::vector<std::string> lines;
};
