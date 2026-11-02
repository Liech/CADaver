#pragma once

#include "UIElement/Radio/RadioUIElement.h"

// Radio button UI element for selecting camera control mode (Arcball, Freecam).
class CameraModeRadioElement : public RadioUIElement
{
  public:
    CameraModeRadioElement(int targetMode, GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows);
    virtual ~CameraModeRadioElement();

    std::string getDescription() const override;
    void        click() override;
    bool        isSelected() const override;

    int getTargetMode() const;

  private:
    static std::string getModeLabel(int mode);

    int targetMode;
};
