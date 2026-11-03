#pragma once

#include <string>

// Parser for application command line startup arguments.
class CommandLineArgs
{
  public:
    CommandLineArgs();
    virtual ~CommandLineArgs();

    static CommandLineArgs parse(int argc, char* argv[]);

    bool               hasScript() const;
    const std::string& getScriptPath() const;
    const std::string& getLanguage() const;
    bool               isVerbose() const;
    const std::string& getOutputPath() const;

    void setScriptPath(const std::string& path);
    void setLanguage(const std::string& lang);
    void setVerbose(bool flag);
    void setOutputPath(const std::string& path);

  private:
    std::string scriptPath;
    std::string language;
    std::string outputPath;
    bool        verbose = false;
};
