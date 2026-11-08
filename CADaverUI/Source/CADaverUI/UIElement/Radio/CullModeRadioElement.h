#pragma once

#include "UIElement/Radio/RadioUIElement.h"

// Radio button UI element for selecting geometry face culling mode (None, Front, Back).
class CullModeRadioElement : public RadioUIElement
{
  public:
    CullModeRadioElement(int targetMode, GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows);
    virtual ~CullModeRadioElement();

    std::string getDescription() const override;
    void        click() override;
    bool        isSelected() const override;

    int getTargetMode() const;

  private:
    static std::string getModeLabel(int mode);

    int targetMode;
};
