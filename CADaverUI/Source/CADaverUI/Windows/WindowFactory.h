#pragma once

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class Window;

// Factory responsible for instantiating registered Window objects by type name.
class WindowFactory
{
  public:
    WindowFactory();
    virtual ~WindowFactory();

    std::shared_ptr<Window>              create(const std::string& type) const;
    std::vector<std::shared_ptr<Window>> getAllWindows() const;

    void registerWindow(const std::string& name, std::function<std::shared_ptr<Window>()> creator);

  private:
    std::unordered_map<std::string, std::function<std::shared_ptr<Window>()>> creators;
    std::vector<std::string>                                                  registeredOrder;
};
