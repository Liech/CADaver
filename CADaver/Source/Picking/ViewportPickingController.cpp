#include "Picking/ViewportPickingController.h"

#include "CADacomb/Util/Ray.h"
#include "Camera/CameraController.h"
#include "Execution/GatedDatabase.h"
#include "Execution/UndoStack.h"
#include "UI/ContextMenu/AssetContextMenuRenderer.h"
#include "UI/Gizmo/AssetGizmoController.h"
#include "Windows/Windows.h"

#include <imgui.h>
#include <ImGuizmo.h>
#include <nlohmann/json.hpp>

ViewportPickingController::ViewportPickingController()
  : contextMenuRenderer(std::make_unique<AssetContextMenuRenderer>())
  , gizmoController(std::make_unique<AssetGizmoController>())
{
}

ViewportPickingController::~ViewportPickingController() {}

void ViewportPickingController::update(GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows, CameraController& cameraController, const Vulcant::Toolkit::SceneData& sceneData)
{
    const float          dragThreshold = 4.0f;
    ImGuiIO&             io            = ImGui::GetIO();
    const ImGuiViewport* viewport      = ImGui::GetMainViewport();
    glm::vec2            mousePos(io.MousePos.x - viewport->Pos.x, io.MousePos.y - viewport->Pos.y);

    if (!io.WantCaptureMouse && !ImGuizmo::IsOver() && !ImGuizmo::IsUsing())
    {
        if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
        {
            CADacomb::Ray  ray      = CADacomb::Ray::fromScreen(mousePos, sceneData.resolution, sceneData.invViewMatrix, sceneData.invProjectionMatrix);
            Polyglot::Node pickRes  = gatedDatabase.execute(Polyglot::Node(nlohmann::json{
              {      "type",                                                          "PickAsset" },
              {   "version",                                                                    1 },
              {    "origin",          { ray.getOrigin().x, ray.getOrigin().y, ray.getOrigin().z } },
              { "direction", { ray.getDirection().x, ray.getDirection().y, ray.getDirection().z } }
            }));
            nlohmann::json pickJson = pickRes.toJson();

            if (pickJson.value("hit", false))
            {
                auto       hitPointJson = pickJson.at("hitPoint");
                glm::dvec3 hitPoint(hitPointJson.at(0).get<double>(), hitPointJson.at(1).get<double>(), hitPointJson.at(2).get<double>());
                cameraController.setFocusPoint(hitPoint);
            }
        }

        if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
        {
            ImVec2 delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left, 0.0f);
            if (delta.x * delta.x + delta.y * delta.y <= dragThreshold * dragThreshold)
            {
                CADacomb::Ray  ray      = CADacomb::Ray::fromScreen(mousePos, sceneData.resolution, sceneData.invViewMatrix, sceneData.invProjectionMatrix);
                Polyglot::Node pickRes  = gatedDatabase.execute(Polyglot::Node(nlohmann::json{
                  {      "type",                                                          "PickAsset" },
                  {   "version",                                                                    1 },
                  {    "origin",          { ray.getOrigin().x, ray.getOrigin().y, ray.getOrigin().z } },
                  { "direction", { ray.getDirection().x, ray.getDirection().y, ray.getDirection().z } }
                }));
                nlohmann::json pickJson = pickRes.toJson();

                if (pickJson.value("hit", false))
                {
                    gatedDatabase.execute({
                      {       "type",                                             "SelectAsset" },
                      {    "version",                                                         1 },
                      {  "assetGuid",               pickJson.at("assetGuid").get<std::string>() },
                      {       "mode", io.KeyCtrl ? "Remove" : (io.KeyShift ? "Add" : "Replace") },
                      { "subObjects",                                                   nullptr }
                    });
                }
                else if (!io.KeyShift && !io.KeyCtrl)
                {
                    gatedDatabase.execute({
                      {    "type", "ClearSelection" },
                      { "version",                1 }
                    });
                }
            }
        }

        if (ImGui::IsMouseReleased(ImGuiMouseButton_Right))
        {
            ImVec2 delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right, 0.0f);
            if (delta.x * delta.x + delta.y * delta.y <= dragThreshold * dragThreshold)
            {
                CADacomb::Ray  ray      = CADacomb::Ray::fromScreen(mousePos, sceneData.resolution, sceneData.invViewMatrix, sceneData.invProjectionMatrix);
                Polyglot::Node pickRes  = gatedDatabase.execute(Polyglot::Node(nlohmann::json{
                  {      "type",                                                          "PickAsset" },
                  {   "version",                                                                    1 },
                  {    "origin",          { ray.getOrigin().x, ray.getOrigin().y, ray.getOrigin().z } },
                  { "direction", { ray.getDirection().x, ray.getDirection().y, ray.getDirection().z } }
                }));
                nlohmann::json pickJson = pickRes.toJson();

                if (pickJson.value("hit", false))
                {
                    contextMenuGuid     = CADacomb::GUID(pickJson.at("assetGuid").get<std::string>());
                    contextMenuType     = pickJson.at("assetType").get<std::string>();
                    auto hitPointJson   = pickJson.at("hitPoint");
                    contextMenuHitPoint = glm::dvec3(hitPointJson.at(0).get<double>(), hitPointJson.at(1).get<double>(), hitPointJson.at(2).get<double>());

                    Polyglot::Node selCheck = gatedDatabase.execute({
                      {      "type",        "GetAssetSelection" },
                      {   "version",                          1 },
                      { "assetGuid", contextMenuGuid.toString() }
                    });

                    if (selCheck.toJson().value("type", "") != "GetAssetSelection")
                    {
                        gatedDatabase.execute({
                          {       "type",              "SelectAsset" },
                          {    "version",                          1 },
                          {  "assetGuid", contextMenuGuid.toString() },
                          {       "mode",                  "Replace" },
                          { "subObjects",                    nullptr }
                        });
                    }

                    ImGui::OpenPopup("ViewportAssetContextMenu");
                }
            }
        }
    }

    if (!contextMenuGuid.toString().empty())
    {
        contextMenuRenderer->renderPopup(gatedDatabase, windows, contextMenuGuid, contextMenuType, "ViewportAssetContextMenu", *gizmoController, contextMenuHitPoint);
    }

    gizmoController->render(gatedDatabase, undoStack, sceneData);
}
