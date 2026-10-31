#pragma once

#include "Windows/Window.h"

#include <string>
#include <utility>
#include <vector>

// UI window presenting available commands and their syntax documentation.
class CommandsWindow : public Window
{
  public:
    CommandsWindow();
    virtual ~CommandsWindow();

    virtual std::string getType() const override;
    virtual std::string getDescription() const override;
    virtual void        render() override;
    virtual bool        isOpen() const override;
    virtual void        open() override;
    virtual void        close() override;

  private:
    void loadData();

    std::vector<std::pair<std::string, std::string>> entries;
    int                                              selectedIndex = 0;
    bool                                             openState     = false;
    bool                                             loaded        = false;
};
