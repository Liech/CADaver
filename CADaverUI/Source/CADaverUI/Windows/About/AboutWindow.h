#pragma once

#include "Windows/Window.h"

#include <string>
#include <utility>
#include <vector>

// UI window presenting project information, credits, and crawled third-party dependency licenses.
class AboutWindow : public Window
{
  public:
    AboutWindow();
    virtual ~AboutWindow();

    virtual std::string getType() const override;
    virtual std::string getDescription() const override;
    virtual void        render() override;

  private:
    void        loadData();
    std::string readFileContent(const std::string& filePath) const;

    std::vector<std::pair<std::string, std::string>> entries;
    int                                              selectedIndex = 0;
    bool                                             loaded        = false;
};
