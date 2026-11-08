#pragma once

#include <string>

class CommandLineArgs;

// Headless executor running Python or Lua scripts from command line arguments.
class HeadlessScriptRunner
{
  public:
    HeadlessScriptRunner();
    virtual ~HeadlessScriptRunner();

    int run(const CommandLineArgs& args);

  private:
    std::string readScriptFile(const std::string& path) const;
    std::string determineLanguage(const CommandLineArgs& args) const;
    void        writeOutputFile(const std::string& path, bool success, const std::string& errorMessage) const;
};
