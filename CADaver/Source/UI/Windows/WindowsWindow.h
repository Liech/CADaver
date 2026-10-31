#pragma once

#include <string>
#include <utility>
#include <vector>

// UI window presenting available windows from WindowFactory and their descriptions.
class WindowsWindow
{
  public:
    WindowsWindow();
    virtual ~WindowsWindow();

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
