#pragma once

#include <string>
#include <utility>
#include <vector>

// UI window presenting project information, credits, and crawled third-party dependency licenses.
class AboutWindow
{
  public:
    AboutWindow();
    virtual ~AboutWindow();

    void render();

    bool isOpen() const;
    void setOpen(bool open);

  private:
    void        loadData();
    std::string readFileContent(const std::string& filePath) const;

    std::vector<std::pair<std::string, std::string>> entries;
    int                                              selectedIndex = 0;
    bool                                             open          = false;
    bool                                             loaded        = false;
};
