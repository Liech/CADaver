#pragma once

#include <string>

class GatedDatabase;
class UndoStack;
class Windows;

// Abstract base class for interactive UI elements with direct access to context services.
class UIElement
{
  public:
    UIElement(GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows);
    virtual ~UIElement();

    virtual std::string getType() const        = 0;
    virtual std::string getDescription() const = 0;
    virtual bool        render()               = 0;
    virtual void        click()                = 0;

  protected:
    GatedDatabase& gatedDatabase;
    UndoStack&     undoStack;
    Windows&       windows;
};
