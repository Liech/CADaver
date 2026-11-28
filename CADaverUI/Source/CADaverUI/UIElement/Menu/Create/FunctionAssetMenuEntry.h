#pragma once

#include "UIElement/Menu/MenuEntry.h"

// Menu entry under Create that opens the function asset generator window.
class FunctionAssetMenuEntry : public MenuEntry
{
  public:
    FunctionAssetMenuEntry(GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows);
    virtual ~FunctionAssetMenuEntry();

    virtual std::string getType() const override;
    virtual std::string getDescription() const override;
    virtual void        click() override;
};
