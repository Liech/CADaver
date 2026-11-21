#include "Windows/Asset/RigidBodyEditWindow.h"

#include "Execution/Defer.h"
#include "Execution/GatedDatabase.h"

#include <imgui.h>

RigidBodyEditWindow::RigidBodyEditWindow() {}

RigidBodyEditWindow::~RigidBodyEditWindow() {}

std::string RigidBodyEditWindow::getType() const
{
    return "RigidBodyEdit";
}

std::string RigidBodyEditWindow::getDescription() const
{
    return "Window: RigidBodyEdit\n"
           "Title: RigidBody\n"
           "Purpose: Small dialog for viewing and editing the RigidBody physics component attached to a triangulation or CAD asset, including a checkbox for whether the body is static.\n\n"
           "Parameters:\n"
           "  - Target Asset: ComboBox selecting the asset within the project\n"
           "  - Static: Checkbox controlling whether the rigid body is static (immovable) or dynamic\n\n"
           "Dispatched Commands: AddComponent, UpdateComponent, RemoveComponent";
}

void RigidBodyEditWindow::setTarget(const CADacomb::GUID& targetAssetGuid, const CADacomb::GUID& targetComponentGuid)
{
    assetComboBox.setSelectedGuid(targetAssetGuid);
    currentComponentGuid = targetComponentGuid;
    needsSync            = true;
}

void RigidBodyEditWindow::render()
{
    if (!openState || !gatedDatabase || !defer)
    {
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(320.0f, 180.0f), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("RigidBody", &openState))
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
            ImGui::TextDisabled("No assets available.");
            ImGui::End();
            return;
        }

        Polyglot::Node compResponse = gatedDatabase->execute({
          {          "type",               "GetComponent" },
          {       "version",                            1 },
          {     "assetGuid", selectedAssetGuid.toString() },
          { "componentType",                  "RigidBody" }
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
            ImGui::TextDisabled("No RigidBody attached to this asset.");
            ImGui::Spacing();

            if (ImGui::Button("Add RigidBody", ImVec2(130.0f, 0.0f)))
            {
                defer->defer(
                  [this, selectedAssetGuid]()
                  {
                      Polyglot::Node command = {
                          {          "type",                              "AddComponent" },
                          {       "version",                                           1 },
                          {     "assetGuid",                selectedAssetGuid.toString() },
                          { "componentType",                                 "RigidBody" },
                          {          "name",                                 "RigidBody" },
                          {          "data", { { "Version", 1 }, { "isStatic", false } } }
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
            cachedName   = activeComp.value("name", "RigidBody");
            cachedStatic = activeComp.contains("data") ? activeComp["data"].value("isStatic", false) : false;
            needsSync    = false;
        }

        ImGui::Spacing();
        if (ImGui::Checkbox("Static", &cachedStatic))
        {
            CADacomb::GUID compToUpdate = currentComponentGuid;
            std::string    updatedName  = cachedName.empty() ? "RigidBody" : cachedName;
            bool           updatedStat  = cachedStatic;
            defer->defer(
              [this, selectedAssetGuid, compToUpdate, updatedName, updatedStat]()
              {
                  Polyglot::Node command = {
                      {          "type",                                 "UpdateComponent" },
                      {       "version",                                                 1 },
                      {     "assetGuid",                      selectedAssetGuid.toString() },
                      { "componentGuid",                           compToUpdate.toString() },
                      {          "name",                                       updatedName },
                      {          "data", { { "Version", 1 }, { "isStatic", updatedStat } } }
                  };
                  gatedDatabase->execute(command);
              });
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

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("RigidBodyEditWindow/TypeAndDescription")
{
    RigidBodyEditWindow window;
    CHECK(window.getType() == "RigidBodyEdit");
    CHECK(window.getDescription().find("Window: RigidBodyEdit") != std::string::npos);
    CHECK(window.getDescription().find("Static") != std::string::npos);
}
#endif
