#pragma once

#include "UIElement/Menu/MenuEntry.h"

#include <string>

// Menu leaf entry under File/New for generating a base shape or opening the generator dialog.
class BaseShapeMenuEntry : public MenuEntry
{
  public:
    BaseShapeMenuEntry(const std::string& shapeId, const std::string& typeName, const std::string& label, const std::string& menuPath, GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows);
    virtual ~BaseShapeMenuEntry();

    virtual std::string getType() const override;
    virtual std::string getDescription() const override;
    virtual void        click() override;

  private:
    std::string shapeId;
    std::string typeName;
};
