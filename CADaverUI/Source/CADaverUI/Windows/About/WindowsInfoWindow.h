#pragma once

#include "Windows/Window.h"

#include <string>
#include <utility>
#include <vector>

// UI window presenting available windows from WindowFactory and their descriptions.
class WindowsInfoWindow : public Window
{
  public:
    WindowsInfoWindow();
    virtual ~WindowsInfoWindow();

    virtual std::string getType() const override;
    virtual std::string getDescription() const override;
    virtual void        render() override;

  private:
    void loadData();

    std::vector<std::pair<std::string, std::string>> entries;
    int                                              selectedIndex = 0;
    bool                                             loaded        = false;
};
