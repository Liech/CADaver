#pragma once

#include "UIElement/Menu/MenuEntry.h"

// Menu leaf entry for loading STL triangulated mesh assets.
class LoadStlMenuEntry : public MenuEntry
{
  public:
    LoadStlMenuEntry(GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows);
    virtual ~LoadStlMenuEntry();

    virtual std::string getType() const override;
    virtual std::string getDescription() const override;
    virtual void        click() override;
};
