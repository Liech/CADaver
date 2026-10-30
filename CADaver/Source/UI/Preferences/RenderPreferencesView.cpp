#include "RenderPreferencesView.h"

#include "Database/Preferences/RenderPreferences.h"

#include <imgui.h>

RenderPreferencesView::RenderPreferencesView() {}

RenderPreferencesView::~RenderPreferencesView() {}

bool RenderPreferencesView::render(RenderPreferences& preferences)
{
    bool modified = false;

    ImGui::Text("Mesh Display");
    ImGui::Separator();
    ImGui::Spacing();

    glm::vec3 mColor = preferences.getMeshColor();
    if (ImGui::ColorEdit3("Mesh Color", &mColor[0]))
    {
        preferences.setMeshColor(mColor);
        modified = true;
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Text("Wireframe");
    ImGui::Separator();
    ImGui::Spacing();

    const char* wireframeModes[] = { "Off", "Overlay", "Only" };
    int         currentMode      = preferences.getWireframeMode();
    if (ImGui::Combo("Wireframe Mode", &currentMode, wireframeModes, 3))
    {
        preferences.setWireframeMode(currentMode);
        modified = true;
    }

    glm::vec3 wColor = preferences.getWireframeColor();
    if (ImGui::ColorEdit3("Wireframe Color", &wColor[0]))
    {
        preferences.setWireframeColor(wColor);
        modified = true;
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Text("Geometry Culling");
    ImGui::Separator();
    ImGui::Spacing();

    const char* cullModes[] = { "None", "Front", "Back" };
    int         currentCull = preferences.getCullMode();
    if (ImGui::Combo("Cull Mode", &currentCull, cullModes, 3))
    {
        preferences.setCullMode(currentCull);
        modified = true;
    }

    return modified;
}
