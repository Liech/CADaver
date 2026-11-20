#include "UI/Gizmo/AssetGizmoController.h"

#include "Execution/GatedDatabase.h"
#include "Execution/UndoStack.h"
#include "Utility/MatrixJson.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

#include <ImGuizmo.h>
#include <nlohmann/json.hpp>

AssetGizmoController::AssetGizmoController() {}

AssetGizmoController::~AssetGizmoController() {}

void AssetGizmoController::activate(const CADacomb::GUID& guid, const glm::dvec3& hitPoint, GizmoMode mode)
{
    assetGuid       = guid;
    hitPointWorld   = hitPoint;
    active          = true;
    needsRefresh    = true;
    localHitPending = true;
    dragging        = false;
    dragHasCommand  = false;
    overlay.setMode(mode);
}

void AssetGizmoController::render(GatedDatabase& gatedDatabase, UndoStack& undoStack, const Vulcant::Toolkit::SceneData& sceneData)
{
    if (!active)
    {
        return;
    }

    if (overlay.render())
    {
        active = false;
        return;
    }

    if (!dragging && (needsRefresh || undoStack.getUndoHistory().size() != undoCount || undoStack.getRedoHistory().size() != redoCount))
    {
        refresh(gatedDatabase, undoStack);
        if (!active)
        {
            return;
        }
        if (localHitPending)
        {
            localHitPoint   = glm::dvec3(glm::inverse(assetTransform) * glm::dvec4(hitPointWorld, 1.0));
            localHitPending = false;
        }
    }

    GizmoMode mode = overlay.getMode();
    if (!dragging)
    {
        glm::dvec3 pivot = overlay.isRotatingAroundCenter() ? aabbCenter : glm::dvec3(assetTransform * glm::dvec4(localHitPoint, 1.0));
        gizmoStart       = glm::dmat4(glm::mat4(BuildGizmoMatrix(pivot, assetTransform, overlay.isLocalSpace() || mode == GizmoMode::Scale)));
        gizmoCurrent     = gizmoStart;
        assetStart       = assetTransform;
    }

    glm::mat4 view       = sceneData.viewMatrix;
    glm::mat4 projection = sceneData.projectionMatrix;
    if (projection[1][1] < 0.0f)
    {
        projection[1][1] = -projection[1][1];
    }

    ImGuizmo::OPERATION operation = ImGuizmo::TRANSLATE;
    float               snap[3]   = { 1.0f, 1.0f, 1.0f };
    if (mode == GizmoMode::Rotate)
    {
        operation = ImGuizmo::ROTATE;
        snap[0]   = 15.0f;
    }
    else if (mode == GizmoMode::Scale)
    {
        operation = ImGuizmo::SCALE;
        snap[0]   = 0.1f;
        snap[1]   = 0.1f;
        snap[2]   = 0.1f;
    }

    glm::mat4            gizmoMatrix(gizmoCurrent);
    const ImGuiViewport* viewport = ImGui::GetMainViewport();

    ImGuizmo::SetOrthographic(projection[3][3] == 1.0f);
    ImGuizmo::SetDrawlist(ImGui::GetBackgroundDrawList());
    ImGuizmo::SetRect(viewport->Pos.x, viewport->Pos.y, sceneData.resolution.x, sceneData.resolution.y);
    bool manipulated = ImGuizmo::Manipulate(glm::value_ptr(view),
                                            glm::value_ptr(projection),
                                            operation,
                                            overlay.isLocalSpace() ? ImGuizmo::LOCAL : ImGuizmo::WORLD,
                                            glm::value_ptr(gizmoMatrix),
                                            nullptr,
                                            ImGui::GetIO().KeyCtrl ? snap : nullptr);

    bool       isUsing = ImGuizmo::IsUsing();
    glm::dmat4 current(gizmoMatrix);
    if (isUsing && (manipulated || current != gizmoCurrent))
    {
        gizmoCurrent = current;
        if (dragHasCommand)
        {
            undoStack.undo();
        }
        gatedDatabase.execute({
          { "type", "TransformAsset" },
          { "version", 1 },
          { "assetGuid", assetGuid.toString() },
          { "transformation", MatrixJson::toJson(ComposeAssetTransform(gizmoCurrent, gizmoStart, assetStart)) }
        });
        dragHasCommand = true;
        recordUndoSizes(undoStack);
    }
    else if (!isUsing && dragging)
    {
        dragHasCommand = false;
        needsRefresh   = true;
    }
    dragging = isUsing;
}

glm::dmat4 AssetGizmoController::BuildGizmoMatrix(const glm::dvec3& pivot, const glm::dmat4& assetTransform, bool localSpace)
{
    glm::dmat4 matrix(1.0);
    if (localSpace)
    {
        matrix[0] = glm::dvec4(glm::normalize(glm::dvec3(assetTransform[0])), 0.0);
        matrix[1] = glm::dvec4(glm::normalize(glm::dvec3(assetTransform[1])), 0.0);
        matrix[2] = glm::dvec4(glm::normalize(glm::dvec3(assetTransform[2])), 0.0);
    }
    matrix[3] = glm::dvec4(pivot, 1.0);
    return matrix;
}

glm::dmat4 AssetGizmoController::ComposeAssetTransform(const glm::dmat4& gizmoCurrent, const glm::dmat4& gizmoStart, const glm::dmat4& assetStart)
{
    return gizmoCurrent * glm::inverse(gizmoStart) * assetStart;
}

void AssetGizmoController::refresh(GatedDatabase& gatedDatabase, UndoStack& undoStack)
{
    Polyglot::Node transformResult = gatedDatabase.execute({
      {      "type", "GetTransformationAsset" },
      {   "version",                        1 },
      { "assetGuid",     assetGuid.toString() }
    });
    if (transformResult["type"].asString() != "GetTransformationAsset")
    {
        active = false;
        return;
    }
    assetTransform = MatrixJson::fromJson(transformResult["transformation"].toJson());

    nlohmann::json aabb = gatedDatabase
                            .execute({
                              {      "type",       "GetAssetAABB" },
                              {   "version",                    1 },
                              { "assetGuid", assetGuid.toString() }
    })
                            .toJson();
    glm::dvec3     minCorner(aabb.at("min").at(0).get<double>(), aabb.at("min").at(1).get<double>(), aabb.at("min").at(2).get<double>());
    glm::dvec3     maxCorner(aabb.at("max").at(0).get<double>(), aabb.at("max").at(1).get<double>(), aabb.at("max").at(2).get<double>());
    aabbCenter   = (minCorner + maxCorner) * 0.5;
    needsRefresh = false;
    recordUndoSizes(undoStack);
}

void AssetGizmoController::recordUndoSizes(UndoStack& undoStack)
{
    undoCount = undoStack.getUndoHistory().size();
    redoCount = undoStack.getRedoHistory().size();
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("AssetGizmoController/ComposeAppliesWorldDeltaToAsset")
{
    glm::dmat4 assetStart = glm::translate(glm::dmat4(1.0), glm::dvec3(1.0, 0.0, 0.0));
    glm::dmat4 gizmoStart = glm::translate(glm::dmat4(1.0), glm::dvec3(2.0, 0.0, 0.0));
    glm::dmat4 current    = glm::translate(glm::dmat4(1.0), glm::dvec3(2.0, 3.0, 0.0));

    glm::dmat4 result = AssetGizmoController::ComposeAssetTransform(current, gizmoStart, assetStart);
    CHECK(result[3] == glm::dvec4(1.0, 3.0, 0.0, 1.0));
}

TEST_CASE("AssetGizmoController/BuildGizmoMatrixWorldHasIdentityBasis")
{
    glm::dmat4 asset  = glm::rotate(glm::dmat4(1.0), 1.0, glm::dvec3(0.0, 0.0, 1.0));
    glm::dmat4 matrix = AssetGizmoController::BuildGizmoMatrix(glm::dvec3(4.0, 5.0, 6.0), asset, false);
    CHECK(glm::dvec3(matrix[0]) == glm::dvec3(1.0, 0.0, 0.0));
    CHECK(matrix[3] == glm::dvec4(4.0, 5.0, 6.0, 1.0));
}
#endif
