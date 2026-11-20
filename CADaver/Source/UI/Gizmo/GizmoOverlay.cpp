#include "UI/Gizmo/GizmoOverlay.h"

#include <imgui.h>

GizmoOverlay::GizmoOverlay() {}

GizmoOverlay::~GizmoOverlay() {}

bool GizmoOverlay::render()
{
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x + viewport->Size.x * 0.5f, viewport->Pos.y + 40.0f), ImGuiCond_Always, ImVec2(0.5f, 0.0f));

    bool done = false;
    if (ImGui::Begin("Move##AssetGizmoOverlay", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings))
    {
        if (ImGui::RadioButton("Translate", mode == GizmoMode::Translate))
        {
            mode = GizmoMode::Translate;
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("Rotate", mode == GizmoMode::Rotate))
        {
            mode = GizmoMode::Rotate;
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("Scale", mode == GizmoMode::Scale))
        {
            mode = GizmoMode::Scale;
        }

        ImGui::Checkbox("Pivot at object center", &rotateAroundCenter);
        ImGui::Checkbox("Local space", &localSpace);

        done = ImGui::Button("Done");
    }
    ImGui::End();
    return done;
}

GizmoMode GizmoOverlay::getMode() const
{
    return mode;
}

void GizmoOverlay::setMode(GizmoMode newMode)
{
    mode = newMode;
}

bool GizmoOverlay::isRotatingAroundCenter() const
{
    return rotateAroundCenter;
}

bool GizmoOverlay::isLocalSpace() const
{
    return localSpace;
}
