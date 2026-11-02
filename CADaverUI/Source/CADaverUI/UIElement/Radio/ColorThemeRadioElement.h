#pragma once

#include "UIElement/Radio/RadioUIElement.h"

// Radio button UI element for selecting UI color theme (Dark, Light, Classic).
class ColorThemeRadioElement : public RadioUIElement
{
  public:
    ColorThemeRadioElement(int targetTheme, GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows);
    virtual ~ColorThemeRadioElement();

    std::string getDescription() const override;
    void        click() override;
    bool        isSelected() const override;

    int getTargetTheme() const;

  private:
    static std::string getThemeLabel(int theme);

    int targetTheme;
};
