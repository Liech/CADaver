#pragma once

#include <string>
#include <utility>
#include <vector>

class GatedDatabase;

// UI window presenting available commands and their syntax documentation.
class CommandsWindow
{
  public:
    CommandsWindow();
    virtual ~CommandsWindow();

    void render(const GatedDatabase& gatedDatabase);

    bool isOpen() const;
    void setOpen(bool open);

  private:
    void loadData(const GatedDatabase& gatedDatabase);

    std::vector<std::pair<std::string, std::string>> entries;
    int                                              selectedIndex = 0;
    bool                                             open          = false;
    bool                                             loaded        = false;
};
