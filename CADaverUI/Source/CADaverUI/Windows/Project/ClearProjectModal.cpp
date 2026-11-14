#include "Windows/Project/ClearProjectModal.h"

#include "Database/Database.h"
#include "Database/Project.h"
#include "Execution/Defer.h"
#include "Execution/GatedDatabase.h"

#include <imgui.h>

ClearProjectModal::ClearProjectModal() {}

ClearProjectModal::~ClearProjectModal() {}

std::string ClearProjectModal::getType() const
{
    return "ClearProject";
}

std::string ClearProjectModal::getDescription() const
{
    return "Window: ClearProject\n"
           "Title: Clear Project\n"
           "Purpose: Confirmation dialog for removing all loaded and generated assets from the active project.\n\n"
           "Parameters:\n"
           "  - Confirmation: Yes / Cancel buttons\n\n"
           "Dispatched Command: ClearProject (sync)";
}

void ClearProjectModal::executeClear()
{
    if (!gatedDatabase || !defer)
    {
        return;
    }

    GatedDatabase* db = gatedDatabase;
    defer->defer(
      [db]()
      {
          Polyglot::Node command = {
              {    "type", "ClearProject" },
              { "version",              1 }
          };
          db->execute(command);
      });
}

void ClearProjectModal::render()
{
    if (!openState || !gatedDatabase)
    {
        return;
    }

    if (ImGui::Begin("Clear Project##ClearProjectWindow", &openState, ImGuiWindowFlags_AlwaysAutoResize))
    {
        Polyglot::Node countRes   = gatedDatabase->execute({
          {    "type", "GetAssetCount" },
          { "version",               1 }
        });
        size_t         assetCount = static_cast<size_t>(countRes["count"].asInt());

        ImGui::Text("Are you sure you want to clear the project?");
        ImGui::TextDisabled("All assets will be removed from the project.");
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::BeginDisabled(assetCount == 0);
        if (ImGui::Button("Yes", ImVec2(120.0f, 0.0f)))
        {
            executeClear();
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
