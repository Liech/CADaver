#pragma once

class RenderPreferences;

// Renders mesh color, wireframe styling, and face culling settings using ImGui.
class RenderPreferencesView
{
  public:
    RenderPreferencesView();
    virtual ~RenderPreferencesView();

    bool render(RenderPreferences& preferences);
};
