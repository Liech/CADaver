#pragma once

#include <string>

class Defer;
class GatedDatabase;
class Windows;

// Base class for UI windows managed by the application.
class Window
{
  public:
    Window();
    virtual ~Window();

    void setInput(GatedDatabase& gatedDatabaseInput, Defer& deferInput, Windows& windowsInput);

    virtual std::string getType() const        = 0;
    virtual std::string getDescription() const = 0;
    virtual void        render()               = 0;

    bool isOpen() const;
    void open();
    void close();
    bool consumeFocus();
    void handleEscapeClose();

  protected:
    GatedDatabase* gatedDatabase = nullptr;
    Defer*         defer         = nullptr;
    Windows*       windows       = nullptr;
    bool           openState     = false;

  private:
    bool shouldFocus = false;
};
