#include "UI/ContextMenu/AssetContextMenuRenderer.h"

#include "Execution/GatedDatabase.h"
#include "UI/ContextMenu/AssetContextMenu.h"
#include "UI/ContextMenu/AssetContextMenuFactory.h"
#include "UI/Gizmo/AssetGizmoController.h"
#include "Windows/Asset/TextNoteEditWindow.h"
#include "Windows/Project/RemoveAssetModal.h"
#include "Windows/Windows.h"

#include <imgui.h>

AssetContextMenuRenderer::AssetContextMenuRenderer()
  : factory(std::make_unique<AssetContextMenuFactory>())
{
}

AssetContextMenuRenderer::~AssetContextMenuRenderer() {}

void AssetContextMenuRenderer::render(GatedDatabase& gatedDatabase, Windows& windows, const CADacomb::GUID& assetGuid, const std::string& assetType)
{
    std::string popupId = "AssetContextMenu##" + assetGuid.toString();
    if (ImGui::BeginPopupContextItem(popupId.c_str()))
    {
        renderMenuContents(gatedDatabase, windows, assetGuid, assetType, nullptr, glm::dvec3(0.0));
        ImGui::EndPopup();
    }
}

void AssetContextMenuRenderer::renderPopup(GatedDatabase&        gatedDatabase,
                                           Windows&              windows,
                                           const CADacomb::GUID& assetGuid,
                                           const std::string&    assetType,
                                           const std::string&    popupId,
                                           AssetGizmoController& gizmoController,
                                           const glm::dvec3&     hitPoint)
{
    if (ImGui::BeginPopup(popupId.c_str()))
    {
        renderMenuContents(gatedDatabase, windows, assetGuid, assetType, &gizmoController, hitPoint);
        ImGui::EndPopup();
    }
}

void AssetContextMenuRenderer::renderMenuContents(GatedDatabase&        gatedDatabase,
                                                  Windows&              windows,
                                                  const CADacomb::GUID& assetGuid,
                                                  const std::string&    assetType,
                                                  AssetGizmoController* gizmoController,
                                                  const glm::dvec3&     hitPoint)
{
    auto menu = factory->create(assetType);
    if (menu)
    {
        menu->render(gatedDatabase, windows, assetGuid);
    }

    if (gizmoController && ImGui::BeginMenu("Move"))
    {
        if (ImGui::MenuItem("Translate"))
        {
            gizmoController->activate(assetGuid, hitPoint, GizmoMode::Translate);
        }
        if (ImGui::MenuItem("Rotate"))
        {
            gizmoController->activate(assetGuid, hitPoint, GizmoMode::Rotate);
        }
        if (ImGui::MenuItem("Scale"))
        {
            gizmoController->activate(assetGuid, hitPoint, GizmoMode::Scale);
        }
        ImGui::EndMenu();
    }

    if (ImGui::MenuItem("Add Note..."))
    {
        Polyglot::Node command = {
            {          "type",                       "AddComponent" },
            {       "version",                                    1 },
            {     "assetGuid",                 assetGuid.toString() },
            { "componentType",                           "TextNote" },
            {          "name",                               "Note" },
            {          "data", { { "Version", 1 }, { "text", "" } } }
        };
        Polyglot::Node result = gatedDatabase.execute(command);
        if (result["type"].asString() == "AddComponent")
        {
            windows.open("TextNoteEdit");
            auto editWindow = std::dynamic_pointer_cast<TextNoteEditWindow>(windows.get("TextNoteEdit"));
            if (editWindow)
            {
                editWindow->setTarget(assetGuid, CADacomb::GUID(result["componentGuid"].asString()));
            }
        }
    }

    ImGui::Separator();
    if (ImGui::MenuItem("Remove Asset"))
    {
        windows.open("RemoveAsset");
        auto modal = std::dynamic_pointer_cast<RemoveAssetModal>(windows.get("RemoveAsset"));
        if (modal)
        {
            modal->setTargetAsset(assetGuid);
        }
    }
}
