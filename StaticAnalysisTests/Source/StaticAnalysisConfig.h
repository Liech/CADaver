#pragma once

#include <string>

// Holds runtime configuration such as the target directory for static analysis scans.
class StaticAnalysisConfig
{
  public:
    StaticAnalysisConfig();
    virtual ~StaticAnalysisConfig();

    static std::string getTargetDirectory();
    static void        setTargetDirectory(const std::string& directory);

  private:
    static std::string targetDirectory;
};
