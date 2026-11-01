#pragma once

#include "Execution/GatedDatabase.h"
#include "Execution/UndoStack.h"
#include "UIElement/UIElement.h"
#include "Windows/Windows.h"

// Safe null-object fallback for unknown or missing UI elements.
class NullUIElement : public UIElement
{
  public:
    NullUIElement();
    virtual ~NullUIElement();

    virtual std::string getType() const override;
    virtual std::string getDescription() const override;
    virtual bool        render() override;
    virtual void        click() override;

  private:
    static GatedDatabase dummyDatabase;
    static UndoStack     dummyUndoStack;
    static Windows       dummyWindows;
};
