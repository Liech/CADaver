#pragma once

#include <string>
#include <utility>
#include <vector>

class CommandFactory;

// UI window presenting available commands and their syntax documentation.
class CommandsWindow
{
  public:
    CommandsWindow();
    virtual ~CommandsWindow();

    void render();

    bool isOpen() const;
    void setOpen(bool open);

  private:
    void loadData();

    std::vector<std::pair<std::string, std::string>> entries;
    int                                              selectedIndex = 0;
    bool                                             open          = false;
    bool                                             loaded        = false;
};
