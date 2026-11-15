#include "Windows/Geometry/ClusterByNormalWindow.h"

#include "Execution/Defer.h"
#include "Execution/GatedDatabase.h"

#include <imgui.h>

ClusterByNormalWindow::ClusterByNormalWindow() : assetComboBox("Triangulation")
{
}

ClusterByNormalWindow::~ClusterByNormalWindow()
{
}

std::string ClusterByNormalWindow::getType() const
{
    return "ClusterByNormal";
}

std::string ClusterByNormalWindow::getDescription() const
{
    return "Window: ClusterByNormal\n"
           "Title: Cluster by Normal\n"
           "Purpose: Non-modal dialog for clustering faces of a Triangulation asset by normal angle threshold.\n\n"
           "Parameters:\n"
           "  - Target Asset: Triangulation asset to cluster\n"
           "  - Max Angle (deg): Maximum angle between adjacent face normals\n"
           "  - Without Holes: Restrict clustering to hole-free manifold regions\n\n"
           "Dispatched Command: ClusterByNormal (async)";
}

void ClusterByNormalWindow::render()
{
    if (!openState || !gatedDatabase)
    {
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(420.0f, 175.0f), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Cluster by Normal", &openState))
    {
        handleEscapeClose();

        assetComboBox.render(*gatedDatabase);

        ImGui::SliderFloat("Max Angle (deg)", &maxAngleDegrees, 0.0f, 180.0f, "%.1f");
        ImGui::Checkbox("Without Holes", &withoutHoles);

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        bool canCluster = !assetComboBox.getSelectedGuid().toString().empty();
        ImGui::BeginDisabled(!canCluster);
        if (ImGui::Button("Cluster", ImVec2(120.0f, 0.0f)))
        {
            executeCluster();
        }
        ImGui::EndDisabled();

        ImGui::SameLine();
        if (ImGui::Button("Close", ImVec2(120.0f, 0.0f)))
        {
            close();
        }
    }
    ImGui::End();
}

void ClusterByNormalWindow::setTargetAsset(const CADacomb::GUID& assetGuid)
{
    assetComboBox.setSelectedGuid(assetGuid);
}

void ClusterByNormalWindow::executeCluster()
{
    CADacomb::GUID selectedGuid = assetComboBox.getSelectedGuid();
    if (!gatedDatabase || !defer || selectedGuid.toString().empty())
    {
        return;
    }

    GatedDatabase* db            = gatedDatabase;
    double         angleDegrees  = static_cast<double>(maxAngleDegrees);
    bool           noHoles       = withoutHoles;
    defer->defer(
      [db, selectedGuid, angleDegrees, noHoles]()
      {
          Polyglot::Node command = {
              {            "type",         "ClusterByNormal" },
              {         "version",                         1 },
              {       "assetGuid",   selectedGuid.toString() },
              { "maxAngleDegrees",              angleDegrees },
              {    "withoutHoles",                   noHoles }
          };
          db->executeAsync(command);
      });
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("ClusterByNormalWindow/TypeAndOpenState")
{
    ClusterByNormalWindow window;
    CHECK(window.getType() == "ClusterByNormal");
    CHECK(!window.isOpen());
    window.open();
    CHECK(window.isOpen());
    window.close();
    CHECK(!window.isOpen());
}
#endif
