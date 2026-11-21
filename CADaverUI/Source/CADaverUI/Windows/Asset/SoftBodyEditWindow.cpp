#include "Windows/Asset/SoftBodyEditWindow.h"

#include "Execution/Defer.h"
#include "Execution/GatedDatabase.h"

#include <algorithm>
#include <imgui.h>

SoftBodyEditWindow::SoftBodyEditWindow()
  : assetComboBox("Triangulation")
{
}

SoftBodyEditWindow::~SoftBodyEditWindow() {}

std::string SoftBodyEditWindow::getType() const
{
    return "SoftBodyEdit";
}

std::string SoftBodyEditWindow::getDescription() const
{
    return "Window: SoftBodyEdit\n"
           "Title: SoftBody\n"
           "Purpose: Dialog for viewing and editing the deformable SoftBody physics component attached to a triangulation asset.\n\n"
           "Parameters:\n"
           "  - Target Asset: ComboBox selecting the triangulation asset within the project\n"
           "  - Pressure: Internal gas/volume pressure resistance (Pa)\n"
           "  - Edge Compliance: Surface stretch compliance (inverse stiffness)\n"
           "  - Bend Compliance: Surface bending compliance (inverse stiffness)\n"
           "  - Mass: Total soft body mass (kg)\n"
           "  - Damping: Linear velocity damping\n"
           "  - Friction: Surface contact friction\n"
           "  - Restitution: Surface contact bounciness [0..1]\n"
           "  - Solver Iterations: XPBD constraint solver iterations per step\n\n"
           "Dispatched Commands: AddComponent, UpdateComponent, RemoveComponent";
}

void SoftBodyEditWindow::setTarget(const CADacomb::GUID& targetAssetGuid, const CADacomb::GUID& targetComponentGuid)
{
    assetComboBox.setSelectedGuid(targetAssetGuid);
    currentComponentGuid = targetComponentGuid;
    needsSync            = true;
}

void SoftBodyEditWindow::render()
{
    if (!openState || !gatedDatabase || !defer)
    {
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(370.0f, 340.0f), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("SoftBody", &openState))
    {
        handleEscapeClose();
        if (assetComboBox.render(*gatedDatabase))
        {
            currentComponentGuid = CADacomb::GUID("");
            needsSync            = true;
        }
        CADacomb::GUID selectedAssetGuid = assetComboBox.getSelectedGuid();

        if (selectedAssetGuid.toString().empty())
        {
            ImGui::Spacing();
            ImGui::TextDisabled("No triangulation assets available.");
            ImGui::End();
            return;
        }

        Polyglot::Node compResponse = gatedDatabase->execute({
          {          "type",               "GetComponent" },
          {       "version",                            1 },
          {     "assetGuid", selectedAssetGuid.toString() },
          { "componentType",                   "SoftBody" }
        });
        nlohmann::json compJson     = compResponse.toJson();
        if (compJson.value("type", "") != "GetComponent")
        {
            ImGui::End();
            return;
        }

        const auto& components = compJson["components"];
        if (components.empty())
        {
            ImGui::Spacing();
            ImGui::TextDisabled("No SoftBody attached to this asset.");
            ImGui::Spacing();

            if (ImGui::Button("Add SoftBody", ImVec2(130.0f, 0.0f)))
            {
                defer->defer(
                  [this, selectedAssetGuid]()
                  {
                      Polyglot::Node command = {
                          {          "type","AddComponent"                                            },
                          {       "version",                            1 },
                          {     "assetGuid", selectedAssetGuid.toString() },
                          { "componentType",                   "SoftBody" },
                          {          "name",                   "SoftBody" },
                          {          "data",
                           { { "Version", 1 },
                           { "mass", 1.0 },
                           { "pressure", 2000.0 },
                           { "edgeCompliance", 0.0001 },
                           { "bendCompliance", 0.001 },
                           { "damping", 0.2 },
                           { "friction", 0.3 },
                           { "restitution", 0.0 },
                           { "solverIterations", 10 } }                  }
                      };
                      gatedDatabase->execute(command);
                  });
                needsSync = true;
            }

            ImGui::End();
            return;
        }

        int activeIdx = -1;
        for (size_t i = 0; i < components.size(); i++)
        {
            if (CADacomb::GUID(components[i].value("componentGuid", "")) == currentComponentGuid)
            {
                activeIdx = static_cast<int>(i);
                break;
            }
        }

        if (activeIdx < 0)
        {
            activeIdx            = 0;
            currentComponentGuid = CADacomb::GUID(components[0].value("componentGuid", ""));
            needsSync            = true;
        }

        const auto& activeComp = components[static_cast<size_t>(activeIdx)];
        if (needsSync)
        {
            cachedName = activeComp.value("name", "SoftBody");
            if (activeComp.contains("data"))
            {
                const auto& data       = activeComp["data"];
                cachedMass             = static_cast<float>(data.value("mass", 1.0));
                cachedPressure         = static_cast<float>(data.value("pressure", 2000.0));
                cachedEdgeCompliance   = static_cast<float>(data.value("edgeCompliance", 0.0001));
                cachedBendCompliance   = static_cast<float>(data.value("bendCompliance", 0.001));
                cachedDamping          = static_cast<float>(data.value("damping", 0.2));
                cachedFriction         = static_cast<float>(data.value("friction", 0.3));
                cachedRestitution      = static_cast<float>(data.value("restitution", 0.0));
                cachedSolverIterations = data.value("solverIterations", 10);
            }
            needsSync = false;
        }

        ImGui::Spacing();
        bool changed = false;

        if (ImGui::DragFloat("Pressure", &cachedPressure, 25.0f, 0.0f, 100000.0f, "%.1f"))
        {
            cachedPressure = std::max(0.0f, cachedPressure);
            changed        = true;
        }
        if (ImGui::DragFloat("Edge Compliance", &cachedEdgeCompliance, 0.0001f, 0.0f, 1.0f, "%.6f"))
        {
            cachedEdgeCompliance = std::max(0.0f, cachedEdgeCompliance);
            changed              = true;
        }
        if (ImGui::DragFloat("Bend Compliance", &cachedBendCompliance, 0.0005f, 0.0f, 1.0f, "%.6f"))
        {
            cachedBendCompliance = std::max(0.0f, cachedBendCompliance);
            changed              = true;
        }
        if (ImGui::DragFloat("Mass", &cachedMass, 0.05f, 0.001f, 10000.0f, "%.3f"))
        {
            cachedMass = std::max(0.001f, cachedMass);
            changed    = true;
        }
        if (ImGui::DragFloat("Damping", &cachedDamping, 0.01f, 0.0f, 10.0f, "%.2f"))
        {
            cachedDamping = std::max(0.0f, cachedDamping);
            changed       = true;
        }
        if (ImGui::DragFloat("Friction", &cachedFriction, 0.01f, 0.0f, 2.0f, "%.2f"))
        {
            cachedFriction = std::max(0.0f, cachedFriction);
            changed        = true;
        }
        if (ImGui::DragFloat("Restitution", &cachedRestitution, 0.01f, 0.0f, 1.0f, "%.2f"))
        {
            cachedRestitution = std::clamp(cachedRestitution, 0.0f, 1.0f);
            changed           = true;
        }
        if (ImGui::SliderInt("Solver Iterations", &cachedSolverIterations, 1, 50))
        {
            cachedSolverIterations = std::max(1, cachedSolverIterations);
            changed                = true;
        }

        if (changed)
        {
            dispatchUpdate(selectedAssetGuid);
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::Button("Remove", ImVec2(90.0f, 0.0f)))
        {
            CADacomb::GUID compToDelete = currentComponentGuid;
            defer->defer(
              [this, compToDelete, selectedAssetGuid]()
              {
                  Polyglot::Node command = {
                      {          "type",            "RemoveComponent" },
                      {       "version",                            1 },
                      { "componentGuid",      compToDelete.toString() },
                      {     "assetGuid", selectedAssetGuid.toString() }
                  };
                  gatedDatabase->execute(command);
              });
            currentComponentGuid = CADacomb::GUID("");
            needsSync            = true;
        }

        ImGui::SameLine();
        if (ImGui::Button("Close", ImVec2(90.0f, 0.0f)))
        {
            close();
        }
    }
    ImGui::End();
}

void SoftBodyEditWindow::dispatchUpdate(const CADacomb::GUID& selectedAssetGuid)
{
    CADacomb::GUID compToUpdate      = currentComponentGuid;
    std::string    updatedName       = cachedName.empty() ? "SoftBody" : cachedName;
    double         updatedMass       = static_cast<double>(cachedMass);
    double         updatedPressure   = static_cast<double>(cachedPressure);
    double         updatedEdgeComp   = static_cast<double>(cachedEdgeCompliance);
    double         updatedBendComp   = static_cast<double>(cachedBendCompliance);
    double         updatedDamping    = static_cast<double>(cachedDamping);
    double         updatedFriction   = static_cast<double>(cachedFriction);
    double         updatedRest       = static_cast<double>(cachedRestitution);
    int            updatedIterations = cachedSolverIterations;

    defer->defer(
      [this, selectedAssetGuid, compToUpdate, updatedName, updatedMass, updatedPressure, updatedEdgeComp, updatedBendComp, updatedDamping, updatedFriction, updatedRest, updatedIterations]()
      {
          Polyglot::Node command = {
              {          "type","UpdateComponent"                                },
              {       "version",                            1 },
              {     "assetGuid", selectedAssetGuid.toString() },
              { "componentGuid",      compToUpdate.toString() },
              {          "name",                  updatedName },
              {          "data",
               { { "Version", 1 },
               { "mass", updatedMass },
               { "pressure", updatedPressure },
               { "edgeCompliance", updatedEdgeComp },
               { "bendCompliance", updatedBendComp },
               { "damping", updatedDamping },
               { "friction", updatedFriction },
               { "restitution", updatedRest },
               { "solverIterations", updatedIterations } }   }
          };
          gatedDatabase->execute(command);
      });
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("SoftBodyEditWindow/TypeAndDescription")
{
    SoftBodyEditWindow window;
    CHECK(window.getType() == "SoftBodyEdit");
    CHECK(window.getDescription().find("Window: SoftBodyEdit") != std::string::npos);
    CHECK(window.getDescription().find("Pressure") != std::string::npos);
}
#endif
