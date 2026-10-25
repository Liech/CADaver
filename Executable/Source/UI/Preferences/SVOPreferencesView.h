#pragma once

class SVOPreferences;

// Renders SVO LOD meshing and visualization preferences controls using ImGui.
class SVOPreferencesView
{
  public:
    SVOPreferencesView();
    virtual ~SVOPreferencesView();

    bool render(SVOPreferences& preferences);
};
