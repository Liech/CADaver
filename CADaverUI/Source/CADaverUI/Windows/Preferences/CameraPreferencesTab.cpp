#include "Windows/Preferences/CameraPreferencesTab.h"

#include "Database/Database.h"
#include "Database/Preferences/CameraPreferences.h"
#include "Database/Preferences/Preferences.h"
#include "Execution/Defer.h"
#include "Execution/GatedDatabase.h"

#include <glm/glm.hpp>
#include <imgui.h>

CameraPreferencesTab::CameraPreferencesTab() {}

CameraPreferencesTab::~CameraPreferencesTab() {}

std::string CameraPreferencesTab::getTitle() const
{
    return "Camera";
}

bool CameraPreferencesTab::render(GatedDatabase& database, Defer& defer)
{
    CameraPreferences preferences = database.get().getPreferences().getCameraPreferences();
    bool              modified    = false;

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

    int projMode = preferences.isOrthographic() ? 1 : 0;
    if (ImGui::RadioButton("Perspective", &projMode, 0))
    {
        preferences.setOrthographic(false);
        modified = true;
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Orthogonal", &projMode, 1))
    {
        preferences.setOrthographic(true);
        modified = true;
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    float speed = preferences.getCameraSpeed();
    if (ImGui::SliderFloat("Move Speed", &speed, 0.1f, 50.0f, "%.1f"))
    {
        preferences.setCameraSpeed(speed);
        modified = true;
    }

    float rotSpeed = static_cast<float>(preferences.getRotationSensitivity());
    if (ImGui::SliderFloat("Look Sensitivity", &rotSpeed, 0.0001f, 0.05f, "%.4f", ImGuiSliderFlags_Logarithmic))
    {
        preferences.setRotationSensitivity(static_cast<double>(rotSpeed));
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
        modified = true;
    }

    float nearVal = preferences.getNearPlane();
    if (ImGui::SliderFloat("Near Plane", &nearVal, 0.001f, 10.0f, "%.3f", ImGuiSliderFlags_Logarithmic))
    {
        preferences.setNearPlane(nearVal);
        modified = true;
    }

    float farVal = preferences.getFarPlane();
    if (ImGui::SliderFloat("Far Plane", &farVal, 10.0f, 10000.0f, "%.0f", ImGuiSliderFlags_Logarithmic))
    {
        preferences.setFarPlane(farVal);
        modified = true;
    }

    if (modified)
    {
        auto json = preferences.toJson();
        defer.defer(
          [&database, json]()
          {
              database.execute({
                {        "type", "UpdateCameraPreferences" },
                {     "version",                         1 },
                { "preferences",                      json }
              });
          });
    }

    return modified;
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("CameraPreferencesTab/GetTitle")
{
    CameraPreferencesTab tab;
    CHECK(tab.getTitle() == "Camera");
}
#endif
