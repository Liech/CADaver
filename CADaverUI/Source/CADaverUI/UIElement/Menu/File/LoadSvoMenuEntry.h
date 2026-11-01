#pragma once

#include "UIElement/Menu/MenuEntry.h"

// Menu leaf entry for loading SVO voxel assets.
class LoadSvoMenuEntry : public MenuEntry
{
  public:
    LoadSvoMenuEntry(GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows);
    virtual ~LoadSvoMenuEntry();

    virtual std::string getType() const override;
    virtual std::string getDescription() const override;
    virtual void        click() override;
};
