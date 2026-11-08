#include "Windows/Project/RemoveAssetModal.h"

#include "Database/Database.h"
#include "Database/Project.h"
#include "Execution/Defer.h"
#include "Execution/GatedDatabase.h"

#include <imgui.h>

RemoveAssetModal::RemoveAssetModal() {}

RemoveAssetModal::~RemoveAssetModal() {}

std::string RemoveAssetModal::getType() const
{
    return "RemoveAsset";
}

std::string RemoveAssetModal::getDescription() const
{
    return "Window: RemoveAsset\n"
           "Title: Remove Asset\n"
           "Purpose: Confirmation dialog for removing a selected asset from the active project.\n\n"
           "Parameters:\n"
           "  - Target Asset: Asset from the project to remove\n"
           "  - Confirmation: Yes / Cancel buttons\n\n"
           "Dispatched Command: RemoveAsset (sync)";
}

void RemoveAssetModal::setTargetAsset(const CADacomb::GUID& assetGuid)
{
    assetComboBox.setSelectedGuid(assetGuid);
}

void RemoveAssetModal::executeRemove()
{
    if (!gatedDatabase || !defer)
    {
        return;
    }

    GatedDatabase* db         = gatedDatabase;
    CADacomb::GUID targetGuid = assetComboBox.getSelectedGuid();

    defer->defer(
      [db, targetGuid]()
      {
          Polyglot::Node command = {
              {      "type",         "RemoveAsset" },
              {   "version",                     1 },
              { "assetGuid", targetGuid.toString() }
          };
          db->execute(command);
      });
}

void RemoveAssetModal::render()
{
    if (!openState || !gatedDatabase)
    {
        return;
    }

    if (ImGui::Begin("Remove Asset##RemoveAssetWindow", &openState, ImGuiWindowFlags_AlwaysAutoResize))
    {
        handleEscapeClose();
        const auto& project = gatedDatabase->get().getProject();
        assetComboBox.render(project);
        ImGui::Spacing();

        ImGui::Text("Are you sure you want to remove the selected asset?");
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        bool hasTarget = project.hasAsset(assetComboBox.getSelectedGuid());
        ImGui::BeginDisabled(!hasTarget);
        if (ImGui::Button("Yes", ImVec2(120.0f, 0.0f)))
        {
            executeRemove();
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
