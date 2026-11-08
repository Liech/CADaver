#pragma once

#include "Windows/Window.h"

#include <string>
#include <utility>
#include <vector>

// UI window presenting available commands and their syntax documentation.
class CommandsInfoWindow : public Window
{
  public:
    CommandsInfoWindow();
    virtual ~CommandsInfoWindow();

    virtual std::string getType() const override;
    virtual std::string getDescription() const override;
    virtual void        render() override;

    void selectCommand(const std::string& commandType);

  private:
    void loadData();

    std::vector<std::pair<std::string, std::string>> entries;
    int                                              selectedIndex     = 0;
    bool                                             loaded            = false;
    bool                                             scrollToSelection = false;
};
