#pragma once

#include <map>
#include <memory>
#include <set>
#include <string>

class Defer;
class GatedDatabase;
class Window;

// Container managing active Window instances by type name.
class WindowContainer
{
  public:
    WindowContainer();
    virtual ~WindowContainer();

    void setInput(GatedDatabase& gatedDatabaseInput, Defer& deferInput);

    void                    open(const std::string& type);
    void                    remove(const std::string& type);
    std::shared_ptr<Window> get(const std::string& type) const;

    void render();

    std::set<std::string> getOpen() const;

  private:
    std::map<std::string, std::shared_ptr<Window>> windows;
    GatedDatabase*                                 gatedDatabase = nullptr;
    Defer*                                         defer         = nullptr;
};
