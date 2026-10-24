#include "CameraPreferencesView.h"

#include "Preferences/CameraPreferences.h"
#include "Vulcant/Toolkit/Camera.h"

#include <imgui.h>

CameraPreferencesView::CameraPreferencesView() {}

CameraPreferencesView::~CameraPreferencesView() {}

bool CameraPreferencesView::render(CameraPreferences& preferences, Vulcant::Toolkit::Camera* camera)
{
    bool modified = false;

    ImGui::Text("Camera Controls");
    ImGui::Separator();
    ImGui::Spacing();

    int currentMode = preferences.getCameraMode();
    if (ImGui::RadioButton("Arcball (Rotate around model)", &currentMode, 0))
    {
        preferences.requestModeChange(0);
        modified = true;
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Freecam (Fly around)", &currentMode, 1))
    {
        preferences.requestModeChange(1);
        modified = true;
    }

    ImGui::Spacing();

    if (ImGui::Button("Reset Camera"))
    {
        preferences.requestReset();
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    float speed = preferences.getCameraSpeed();
    if (ImGui::SliderFloat("Move Speed", &speed, 0.1f, 50.0f, "%.1f"))
    {
        preferences.setCameraSpeed(speed);
        if (camera)
        {
            camera->setTranslationSensitivity(speed);
        }
        modified = true;
    }

    float rotSpeed = static_cast<float>(preferences.getRotationSensitivity());
    if (ImGui::SliderFloat("Look Sensitivity", &rotSpeed, 0.0001f, 0.05f, "%.4f", ImGuiSliderFlags_Logarithmic))
    {
        preferences.setRotationSensitivity(static_cast<double>(rotSpeed));
        if (camera)
        {
            camera->setRotationSensitivity(preferences.getRotationSensitivity());
        }
        modified = true;
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Text("Frustum & SceneData");
    ImGui::Separator();
    ImGui::Spacing();

    float fovDegrees = glm::degrees(preferences.getFov());
    if (ImGui::SliderFloat("Field of View", &fovDegrees, 10.0f, 120.0f, "%.1f deg"))
    {
        preferences.setFov(glm::radians(fovDegrees));
        if (camera)
        {
            camera->setFov(preferences.getFov());
        }
        modified = true;
    }

    float currentNear = preferences.getNearPlane();
    if (ImGui::DragFloat("Near Plane", &currentNear, 0.01f, 0.001f, 100.0f, "%.3f"))
    {
        preferences.setNearPlane(currentNear);
        if (camera)
        {
            camera->setZNear(preferences.getNearPlane());
        }
        modified = true;
    }

    float currentFar = preferences.getFarPlane();
    if (ImGui::DragFloat("Far Plane", &currentFar, 10.0f, 1.0f, 100000.0f, "%.1f"))
    {
        preferences.setFarPlane(currentFar);
        if (camera)
        {
            camera->setZFar(preferences.getFarPlane());
        }
        modified = true;
    }

    glm::vec2 currentRes = preferences.getResolution();
    if (ImGui::DragFloat2("Resolution", &currentRes.x, 1.0f, 1.0f, 7680.0f, "%.0f"))
    {
        preferences.setResolution(currentRes);
        modified = true;
    }

    ImGui::Spacing();

    if (camera)
    {
        ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.3f, 1.0f), "Mode: %s | Press ESC to toggle look", camera->isActive() ? "Captured (WASD/Mouse)" : "UI Mode (Clickable)");
    }

    return modified;
}
