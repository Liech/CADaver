#pragma once

#include <string>

class Defer;
class GatedDatabase;
class WindowContainer;

// Base class for UI windows managed by the application.
class Window
{
  public:
    Window();
    virtual ~Window();

    void setInput(GatedDatabase& gatedDatabaseInput, Defer& deferInput, WindowContainer& windowContainerInput);

    virtual std::string getType() const        = 0;
    virtual std::string getDescription() const = 0;
    virtual void        render()               = 0;

    bool isOpen() const;
    void open();
    void close();
    bool consumeFocus();

  protected:
    GatedDatabase*   gatedDatabase   = nullptr;
    Defer*           defer           = nullptr;
    WindowContainer* windowContainer = nullptr;
    bool             openState       = false;

  private:
    bool shouldFocus = false;
};
