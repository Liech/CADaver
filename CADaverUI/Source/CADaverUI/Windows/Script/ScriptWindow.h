#pragma once

#include "Windows/Window.h"

#include <string>
#include <vector>

// UI window providing a multi-language script editor and runner for commands.
class ScriptWindow : public Window
{
  public:
    ScriptWindow();
    virtual ~ScriptWindow();

    virtual std::string getType() const override;
    virtual std::string getDescription() const override;
    virtual void        render() override;

    void               setScript(const std::string& script);
    const std::string& getScript() const;

    void setLanguage(int index);
    int  getLanguage() const;

    void runScript();

    const std::string& getOutput() const;
    bool               hasExecutionError() const;

  private:
    void renderControls();
    void renderEditor();
    void renderOutput();

    std::vector<char> scriptBuffer;
    std::string       scriptText;
    std::string       outputMessage;
    int               selectedLanguage = 0;
    bool              hasError         = false;
};
