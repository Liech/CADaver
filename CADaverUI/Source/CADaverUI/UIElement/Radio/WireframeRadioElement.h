#pragma once

#include "UIElement/Radio/RadioUIElement.h"

// Radio button UI element for selecting wireframe rendering mode (Off, Overlay, Only).
class WireframeRadioElement : public RadioUIElement
{
  public:
    WireframeRadioElement(int targetMode, GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows);
    virtual ~WireframeRadioElement();

    std::string getDescription() const override;
    void        click() override;
    bool        isSelected() const override;

    int getTargetMode() const;

  private:
    static std::string getModeLabel(int mode);

    int targetMode;
};
