#pragma once

#include "UIElement/Menu/MenuEntry.h"

// Menu leaf entry for opening the concepts and architecture viewer.
class ConceptsInfoMenuEntry : public MenuEntry
{
  public:
    ConceptsInfoMenuEntry(GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows);
    virtual ~ConceptsInfoMenuEntry();

    virtual std::string getType() const override;
    virtual std::string getDescription() const override;
    virtual void        click() override;
};
