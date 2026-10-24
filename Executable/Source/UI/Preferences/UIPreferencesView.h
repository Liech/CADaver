#pragma once

class UIPreferences;

// Renders interface theme, scale, and background color settings using ImGui.
class UIPreferencesView
{
  public:
    UIPreferencesView();
    virtual ~UIPreferencesView();

    bool render(UIPreferences& preferences);
    void apply(const UIPreferences& preferences);
};
