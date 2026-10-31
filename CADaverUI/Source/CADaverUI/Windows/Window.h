#pragma once

#include <string>

class GatedDatabase;
class Defer;

// Base interface for UI windows managed by the application.
class Window
{
  public:
    Window();
    virtual ~Window();

    void setInput(GatedDatabase& gatedDatabaseInput, Defer& deferInput);

    virtual std::string getType() const        = 0;
    virtual std::string getDescription() const = 0;
    virtual void        render()               = 0;
    virtual bool        isOpen() const         = 0;
    virtual void        open()                 = 0;
    virtual void        close()                = 0;

  protected:
    GatedDatabase* gatedDatabase = nullptr;
    Defer*         defer         = nullptr;
};
