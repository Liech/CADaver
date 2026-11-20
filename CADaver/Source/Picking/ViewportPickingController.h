#pragma once

#include "CADacomb/Util/GUID.h"
#include "Vulcant/Toolkit/SceneData.h"

#include <memory>
#include <string>

class AssetContextMenuRenderer;
class AssetGizmoController;
class CameraController;
class GatedDatabase;
class UndoStack;
class Windows;

// Handles 3D viewport mouse picking, selection updates, and viewport asset context menus via commands.
class ViewportPickingController
{
  public:
    ViewportPickingController();
    virtual ~ViewportPickingController();

    void update(GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows, CameraController& cameraController, const Vulcant::Toolkit::SceneData& sceneData);

  private:
    std::unique_ptr<AssetContextMenuRenderer> contextMenuRenderer;
    std::unique_ptr<AssetGizmoController>     gizmoController;
    CADacomb::GUID                            contextMenuGuid;
    std::string                               contextMenuType;
    glm::dvec3                                contextMenuHitPoint = glm::dvec3(0.0);
};
