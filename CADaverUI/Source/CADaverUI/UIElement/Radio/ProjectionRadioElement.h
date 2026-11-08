#pragma once

#include "UIElement/Radio/RadioUIElement.h"

// Radio button UI element for selecting camera projection mode (Perspective, Orthogonal).
class ProjectionRadioElement : public RadioUIElement
{
  public:
    ProjectionRadioElement(bool orthographic, GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows);
    virtual ~ProjectionRadioElement();

    std::string getDescription() const override;
    void        click() override;
    bool        isSelected() const override;

    bool isOrthographic() const;

  private:
    static std::string getProjectionLabel(bool orthographic);

    bool orthographic;
};
