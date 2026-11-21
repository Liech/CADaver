#pragma once

#include "CADacomb/Util/GUID.h"
#include "UI/Gizmo/GizmoMode.h"
#include "UI/Gizmo/GizmoOverlay.h"
#include "Vulcant/Toolkit/SceneData.h"

#include <glm/glm.hpp>

class GatedDatabase;
class UndoStack;

// Drives an ImGuizmo transform handle for a single asset. Each drag frame undoes the previous TransformAsset of the drag and executes a new one with the full current transformation, so one drag is
// one undo step.
class AssetGizmoController
{
  public:
    AssetGizmoController();
    virtual ~AssetGizmoController();

    void activate(const CADacomb::GUID& assetGuid, const glm::dvec3& hitPoint, GizmoMode mode);
    void render(GatedDatabase& gatedDatabase, UndoStack& undoStack, const Vulcant::Toolkit::SceneData& sceneData);

    static glm::dmat4 BuildGizmoMatrix(const glm::dvec3& pivot, const glm::dmat4& assetTransform, bool localSpace);
    static glm::dmat4 ComposeAssetTransform(const glm::dmat4& gizmoCurrent, const glm::dmat4& gizmoStart, const glm::dmat4& assetStart);

  private:
    void refresh(GatedDatabase& gatedDatabase, UndoStack& undoStack);
    void recordUndoSizes(UndoStack& undoStack);

    GizmoOverlay   overlay;
    CADacomb::GUID assetGuid;
    glm::dvec3     hitPointWorld   = glm::dvec3(0.0);
    glm::dvec3     localHitPoint   = glm::dvec3(0.0);
    glm::dvec3     aabbCenter      = glm::dvec3(0.0);
    glm::dmat4     assetTransform  = glm::dmat4(1.0);
    glm::dmat4     gizmoStart      = glm::dmat4(1.0);
    glm::dmat4     gizmoCurrent    = glm::dmat4(1.0);
    glm::dmat4     assetStart      = glm::dmat4(1.0);
    size_t         undoCount       = 0;
    size_t         redoCount       = 0;
    bool           active          = false;
    bool           needsRefresh    = false;
    bool           localHitPending = false;
    bool           dragging        = false;
    bool           dragHasCommand  = false;
};
