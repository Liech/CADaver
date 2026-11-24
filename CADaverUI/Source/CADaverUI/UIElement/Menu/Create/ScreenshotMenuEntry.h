#pragma once

#include "UIElement/Menu/MenuEntry.h"

// Menu leaf entry that captures the current viewport as a TextureAsset.
class ScreenshotMenuEntry : public MenuEntry
{
  public:
    ScreenshotMenuEntry(GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows);
    virtual ~ScreenshotMenuEntry();

    virtual std::string getType() const override;
    virtual std::string getDescription() const override;
    virtual void        click() override;
};
