#pragma once

#include "UIElement/Menu/MenuEntry.h"

// Menu leaf entry for loading CAD, mesh, or voxel assets.
class LoadAssetMenuEntry : public MenuEntry
{
  public:
    LoadAssetMenuEntry(GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows);
    virtual ~LoadAssetMenuEntry();

    virtual std::string getType() const override;
    virtual std::string getDescription() const override;
    virtual void        click() override;
};
