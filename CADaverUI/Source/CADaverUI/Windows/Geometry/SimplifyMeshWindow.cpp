#include "Windows/Geometry/SimplifyMeshWindow.h"

#include "Execution/Defer.h"
#include "Execution/GatedDatabase.h"

#include <imgui.h>

SimplifyMeshWindow::SimplifyMeshWindow()
  : assetComboBox("Triangulation")
{
}

SimplifyMeshWindow::~SimplifyMeshWindow() {}

std::string SimplifyMeshWindow::getType() const
{
    return "SimplifyMesh";
}

std::string SimplifyMeshWindow::getDescription() const
{
    return "Window: SimplifyMesh\n"
           "Title: Simplify Mesh\n"
           "Purpose: Dialog for simplifying a triangulation mesh asset by percentage or target triangle count.\n\n"
           "Parameters:\n"
           "  - Target Asset: Triangulation asset from the project to simplify\n"
           "  - Mode: By Percent or By Triangle Count\n"
           "  - Target Percent: Target percentage of triangles to keep (1% to 100%)\n"
           "  - Target Triangle Count: Absolute target number of triangles\n"
           "  - Delete original mesh asset: Removes the source mesh asset upon successful simplification\n\n"
           "Dispatched Command: SimplifyMesh (async)";
}

void SimplifyMeshWindow::setTargetAsset(const CADacomb::GUID& assetGuid)
{
    assetComboBox.setSelectedGuid(assetGuid);
}

void SimplifyMeshWindow::render()
{
    if (!openState || !gatedDatabase)
    {
        return;
    }

    if (ImGui::Begin("Simplify Mesh##SimplifyMeshModal", &openState, ImGuiWindowFlags_AlwaysAutoResize))
    {
        assetComboBox.render(*gatedDatabase);
        ImGui::Spacing();

        ImGui::RadioButton("By Percent", &modeIndex, 0);
        ImGui::SameLine();
        ImGui::RadioButton("By Triangle Count", &modeIndex, 1);
        ImGui::Spacing();

        if (modeIndex == 0)
        {
            ImGui::SliderFloat("Target Percent (%)", &targetPercent, 1.0f, 100.0f, "%.1f%%");
        }
        else
        {
            if (ImGui::InputInt("Target Triangles", &targetTriangleCount))
            {
                if (targetTriangleCount < 4)
                {
                    targetTriangleCount = 4;
                }
            }
        }

        ImGui::Checkbox("Delete original mesh asset", &deleteOrigin);
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        bool hasTarget = !assetComboBox.getSelectedGuid().toString().empty();
        ImGui::BeginDisabled(!hasTarget);
        if (ImGui::Button("Simplify", ImVec2(120.0f, 0.0f)))
        {
            executeSimplification();
            close();
        }
        ImGui::EndDisabled();

        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120.0f, 0.0f)))
        {
            close();
        }
    }
    ImGui::End();
}

void SimplifyMeshWindow::executeSimplification()
{
    if (!gatedDatabase || !defer)
    {
        return;
    }

    GatedDatabase* db        = gatedDatabase;
    CADacomb::GUID guid      = assetComboBox.getSelectedGuid();
    std::string    mode      = (modeIndex == 0) ? "Percent" : "TriangleCount";
    double         pct       = static_cast<double>(targetPercent);
    int            triCount  = targetTriangleCount;
    bool           delOrigin = deleteOrigin;

    defer->defer(
      [db, guid, mode, pct, triCount, delOrigin]()
      {
          Polyglot::Node command = {
              {                "type",  "SimplifyMesh" },
              {             "version",               1 },
              {           "assetGuid", guid.toString() },
              {                "mode",            mode },
              {       "targetPercent",             pct },
              { "targetTriangleCount",        triCount },
              {        "deleteOrigin",       delOrigin }
          };
          db->executeAsync(command);
      });
}
