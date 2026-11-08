#include "Windows/Preferences/RenderPreferencesTab.h"

#include "Database/Database.h"
#include "Database/Preferences/Preferences.h"
#include "Database/Preferences/RenderPreferences.h"
#include "Execution/Defer.h"
#include "Execution/GatedDatabase.h"

#include <imgui.h>

RenderPreferencesTab::RenderPreferencesTab() {}

RenderPreferencesTab::~RenderPreferencesTab() {}

std::string RenderPreferencesTab::getTitle() const
{
    return "Render";
}

std::string RenderPreferencesTab::getDescription() const
{
    return "Preference Tab: Render\n"
           "Purpose: Configure 3D viewport rendering options, including mesh color, wireframe mode (Off, Overlay, Only), wireframe color, and backface culling modes.";
}

bool RenderPreferencesTab::render(GatedDatabase& database, Defer& defer)
{
    RenderPreferences preferences = database.get().getPreferences().getRenderPreferences();
    bool              modified    = false;

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

    if (modified)
    {
        Polyglot::Node command = {
            {        "type", "UpdateRenderPreferences" },
            {     "version",                         1 },
            { "preferences",      preferences.toNode() }
        };
        defer.defer([&database, command]() { database.execute(command); });
    }

    return modified;
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("RenderPreferencesTab/GetTitle")
{
    RenderPreferencesTab tab;
    CHECK(tab.getTitle() == "Render");
}
#endif
