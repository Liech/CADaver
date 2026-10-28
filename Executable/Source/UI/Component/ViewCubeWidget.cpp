#include "UI/Component/ViewCubeWidget.h"

#include <imgui.h>

#include <ImGuizmo.h>
#include <algorithm>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Vulcant/Toolkit/Camera.h"
#include "Vulcant/Toolkit/SceneData.h"

ViewCubeWidget::ViewCubeWidget() {}

ViewCubeWidget::~ViewCubeWidget() {}

void ViewCubeWidget::render(Vulcant::Toolkit::Camera* camera, const glm::ivec2& resolution, const std::function<void()>& onHomePressed)
{
    if (!camera || resolution.x <= 0 || resolution.y <= 0)
    {
        return;
    }

    float scale = ImGui::GetIO().FontGlobalScale;
    if (scale < 0.2f)
    {
        scale = 1.0f;
    }

    float cubeSize  = baseCubeSize * scale;
    float margin    = baseMargin * scale;
    float arrowDist = baseArrowDist * scale;

    float menuBarHeight = ImGui::GetFrameHeight();
    float posX          = static_cast<float>(resolution.x) - cubeSize - margin;
    float posY          = menuBarHeight + margin;
    float cx            = posX + cubeSize * 0.5f;
    float cy            = posY + cubeSize * 0.5f;

    glm::dvec3 eye    = camera->getEye();
    glm::dvec3 target = camera->getTarget();
    double     dist   = glm::length(eye - target);
    if (dist < 1e-4)
    {
        dist = 5.0;
    }

    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav |
                                   ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground;

    ImU32 normColor  = IM_COL32(180, 180, 180, 180);
    ImU32 hoverColor = IM_COL32(255, 255, 255, 255);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    // Home button
    ImGui::SetNextWindowPos(ImVec2(cx - 58.0f * scale - 12.0f * scale, cy - arrowDist - 12.0f * scale));
    ImGui::Begin("##ViewCubeHome", nullptr, windowFlags);
    {
        bool hasEmojiGlyph = (sizeof(ImWchar) >= 4 && ImGui::GetFont() && ImGui::GetFont()->IsGlyphInFont(static_cast<ImWchar>(0x1F3E0)));
        bool homeClicked   = false;
        if (hasEmojiGlyph)
        {
            homeClicked = ImGui::Button(reinterpret_cast<const char*>(u8"🏠"), ImVec2(24.0f * scale, 24.0f * scale));
        }
        else
        {
            homeClicked = ImGui::Button("##HomeButton", ImVec2(24.0f * scale, 24.0f * scale));

            ImDrawList* drawList = ImGui::GetWindowDrawList();
            ImVec2      btnMin   = ImGui::GetItemRectMin();
            ImVec2      btnMax   = ImGui::GetItemRectMax();
            ImVec2      center   = ImVec2((btnMin.x + btnMax.x) * 0.5f, (btnMin.y + btnMax.y) * 0.5f);

            ImU32 iconColor = ImGui::GetColorU32(ImGuiCol_Text);
            ImU32 doorColor = ImGui::GetColorU32(ImGuiCol_Button);

            ImVec2 pRoofTop(center.x, center.y - 7.0f * scale);
            ImVec2 pRoofLeft(center.x - 7.0f * scale, center.y - 1.0f * scale);
            ImVec2 pRoofRight(center.x + 7.0f * scale, center.y - 1.0f * scale);
            drawList->AddTriangleFilled(pRoofTop, pRoofLeft, pRoofRight, iconColor);
            drawList->AddRectFilled(ImVec2(center.x + 2.5f * scale, center.y - 6.0f * scale), ImVec2(center.x + 4.5f * scale, center.y - 2.0f * scale), iconColor);
            drawList->AddRectFilled(ImVec2(center.x - 5.0f * scale, center.y - 1.0f * scale), ImVec2(center.x + 5.0f * scale, center.y + 6.0f * scale), iconColor);
            drawList->AddRectFilled(ImVec2(center.x - 1.5f * scale, center.y + 2.0f * scale), ImVec2(center.x + 1.5f * scale, center.y + 6.0f * scale), doorColor);
        }
        if (homeClicked && onHomePressed)
        {
            onHomePressed();
        }
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Home (Reset Camera View)");
        }
    }
    ImGui::End();

    // Roll CCW button
    ImGui::SetNextWindowPos(ImVec2(cx - 34.0f * scale - 10.0f * scale, cy - arrowDist - 10.0f * scale));
    ImGui::Begin("##ViewCubeRollCCW", nullptr, windowFlags);
    if (ImGui::InvisibleButton("##RollCCW", ImVec2(20.0f * scale, 20.0f * scale)))
    {
        rollCamera(camera, glm::radians(90.0));
    }
    bool rollCcwHovered = ImGui::IsItemHovered();
    if (rollCcwHovered)
    {
        ImGui::SetTooltip("Roll 90° Counter-Clockwise");
    }
    {
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImU32       col      = rollCcwHovered ? hoverColor : normColor;
        ImVec2      arcC     = ImVec2(cx - 34.0f * scale, cy - arrowDist);
        drawList->PathArcTo(arcC, 6.0f * scale, glm::radians(140.0f), glm::radians(330.0f), 8);
        drawList->PathStroke(col, 0, 2.0f * scale);
        drawList->AddTriangleFilled(ImVec2(arcC.x - 7.0f * scale, arcC.y - 1.0f * scale), ImVec2(arcC.x - 2.0f * scale, arcC.y - 6.0f * scale), ImVec2(arcC.x - 1.0f * scale, arcC.y), col);
    }
    ImGui::End();

    // Roll CW button
    ImGui::SetNextWindowPos(ImVec2(cx + 34.0f * scale - 10.0f * scale, cy - arrowDist - 10.0f * scale));
    ImGui::Begin("##ViewCubeRollCW", nullptr, windowFlags);
    if (ImGui::InvisibleButton("##RollCW", ImVec2(20.0f * scale, 20.0f * scale)))
    {
        rollCamera(camera, -glm::radians(90.0));
    }
    bool rollCwHovered = ImGui::IsItemHovered();
    if (rollCwHovered)
    {
        ImGui::SetTooltip("Roll 90° Clockwise");
    }
    {
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImU32       col      = rollCwHovered ? hoverColor : normColor;
        ImVec2      arcC     = ImVec2(cx + 34.0f * scale, cy - arrowDist);
        drawList->PathArcTo(arcC, 6.0f * scale, glm::radians(210.0f), glm::radians(400.0f), 8);
        drawList->PathStroke(col, 0, 2.0f * scale);
        drawList->AddTriangleFilled(ImVec2(arcC.x + 7.0f * scale, arcC.y - 1.0f * scale), ImVec2(arcC.x + 2.0f * scale, arcC.y - 6.0f * scale), ImVec2(arcC.x + 1.0f * scale, arcC.y), col);
    }
    ImGui::End();

    // Projection toggle button (Orthogonal / Perspective)
    ImGui::SetNextWindowPos(ImVec2(cx + 58.0f * scale - 12.0f * scale, cy - arrowDist - 12.0f * scale));
    ImGui::Begin("##ViewCubeProjection", nullptr, windowFlags);
    {
        bool isOrtho     = camera->isOrthographic();
        bool projClicked = ImGui::Button("##ProjButton", ImVec2(24.0f * scale, 24.0f * scale));
        if (projClicked)
        {
            camera->setOrthographic(!isOrtho);
        }
        bool projHovered = ImGui::IsItemHovered();
        if (projHovered)
        {
            ImGui::SetTooltip(isOrtho ? "Orthographic (Click for Perspective)" : "Perspective (Click for Orthogonal)");
        }

        ImDrawList* drawList  = ImGui::GetWindowDrawList();
        ImVec2      btnMin    = ImGui::GetItemRectMin();
        ImVec2      btnMax    = ImGui::GetItemRectMax();
        ImVec2      center    = ImVec2((btnMin.x + btnMax.x) * 0.5f, (btnMin.y + btnMax.y) * 0.5f);
        ImU32       iconColor = projHovered ? hoverColor : normColor;

        if (isOrtho)
        {
            float  s = 5.0f * scale;
            ImVec2 top(center.x, center.y - s);
            ImVec2 right(center.x + s * 0.866f, center.y - s * 0.5f);
            ImVec2 left(center.x - s * 0.866f, center.y - s * 0.5f);
            ImVec2 mid(center.x, center.y);
            ImVec2 bot(center.x, center.y + s);
            ImVec2 botRight(center.x + s * 0.866f, center.y + s * 0.5f);
            ImVec2 botLeft(center.x - s * 0.866f, center.y + s * 0.5f);

            drawList->AddLine(top, right, iconColor, 1.5f * scale);
            drawList->AddLine(right, mid, iconColor, 1.5f * scale);
            drawList->AddLine(mid, left, iconColor, 1.5f * scale);
            drawList->AddLine(left, top, iconColor, 1.5f * scale);
            drawList->AddLine(mid, bot, iconColor, 1.5f * scale);
            drawList->AddLine(left, botLeft, iconColor, 1.5f * scale);
            drawList->AddLine(botLeft, bot, iconColor, 1.5f * scale);
            drawList->AddLine(right, botRight, iconColor, 1.5f * scale);
            drawList->AddLine(botRight, bot, iconColor, 1.5f * scale);
        }
        else
        {
            float  s = 6.0f * scale;
            ImVec2 apex(center.x, center.y - s);
            ImVec2 baseLeft(center.x - s * 0.9f, center.y + s * 0.7f);
            ImVec2 baseRight(center.x + s * 0.9f, center.y + s * 0.7f);
            ImVec2 baseMid(center.x, center.y + s);

            drawList->AddTriangle(apex, baseLeft, baseRight, iconColor, 1.5f * scale);
            drawList->AddLine(apex, baseMid, iconColor, 1.5f * scale);
            drawList->AddLine(baseLeft, baseMid, iconColor, 1.5f * scale);
            drawList->AddLine(baseRight, baseMid, iconColor, 1.5f * scale);
        }
    }
    ImGui::End();

    // Up arrow button
    ImGui::SetNextWindowPos(ImVec2(cx - 10.0f * scale, cy - arrowDist - 8.0f * scale));
    ImGui::Begin("##ViewCubeArrowUp", nullptr, windowFlags);
    if (ImGui::InvisibleButton("##ArrowUp", ImVec2(20.0f * scale, 16.0f * scale)))
    {
        glm::dvec3 forward = glm::normalize(target - eye);
        glm::dvec3 right   = glm::normalize(glm::cross(forward, camera->getUp()));
        rotateAroundTarget(camera, glm::radians(90.0), right);
    }
    bool upHovered = ImGui::IsItemHovered();
    if (upHovered)
    {
        ImGui::SetTooltip("Rotate 90° Up");
    }
    {
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImU32       col      = upHovered ? hoverColor : normColor;
        drawList->AddTriangleFilled(ImVec2(cx, cy - arrowDist - 6.0f * scale), ImVec2(cx - 7.0f * scale, cy - arrowDist + 5.0f * scale), ImVec2(cx + 7.0f * scale, cy - arrowDist + 5.0f * scale), col);
    }
    ImGui::End();

    // Down arrow button
    ImGui::SetNextWindowPos(ImVec2(cx - 10.0f * scale, cy + arrowDist - 8.0f * scale));
    ImGui::Begin("##ViewCubeArrowDown", nullptr, windowFlags);
    if (ImGui::InvisibleButton("##ArrowDown", ImVec2(20.0f * scale, 16.0f * scale)))
    {
        glm::dvec3 forward = glm::normalize(target - eye);
        glm::dvec3 right   = glm::normalize(glm::cross(forward, camera->getUp()));
        rotateAroundTarget(camera, -glm::radians(90.0), right);
    }
    bool downHovered = ImGui::IsItemHovered();
    if (downHovered)
    {
        ImGui::SetTooltip("Rotate 90° Down");
    }
    {
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImU32       col      = downHovered ? hoverColor : normColor;
        drawList->AddTriangleFilled(ImVec2(cx, cy + arrowDist + 6.0f * scale), ImVec2(cx - 7.0f * scale, cy + arrowDist - 5.0f * scale), ImVec2(cx + 7.0f * scale, cy + arrowDist - 5.0f * scale), col);
    }
    ImGui::End();

    // Left arrow button
    ImGui::SetNextWindowPos(ImVec2(cx - arrowDist - 8.0f * scale, cy - 10.0f * scale));
    ImGui::Begin("##ViewCubeArrowLeft", nullptr, windowFlags);
    if (ImGui::InvisibleButton("##ArrowLeft", ImVec2(16.0f * scale, 20.0f * scale)))
    {
        rotateAroundTarget(camera, glm::radians(90.0), camera->getUp());
    }
    bool leftHovered = ImGui::IsItemHovered();
    if (leftHovered)
    {
        ImGui::SetTooltip("Rotate 90° Left");
    }
    {
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImU32       col      = leftHovered ? hoverColor : normColor;
        drawList->AddTriangleFilled(ImVec2(cx - arrowDist - 6.0f * scale, cy), ImVec2(cx - arrowDist + 5.0f * scale, cy - 7.0f * scale), ImVec2(cx - arrowDist + 5.0f * scale, cy + 7.0f * scale), col);
    }
    ImGui::End();

    // Right arrow button
    ImGui::SetNextWindowPos(ImVec2(cx + arrowDist - 8.0f * scale, cy - 10.0f * scale));
    ImGui::Begin("##ViewCubeArrowRight", nullptr, windowFlags);
    if (ImGui::InvisibleButton("##ArrowRight", ImVec2(16.0f * scale, 20.0f * scale)))
    {
        rotateAroundTarget(camera, -glm::radians(90.0), camera->getUp());
    }
    bool rightHovered = ImGui::IsItemHovered();
    if (rightHovered)
    {
        ImGui::SetTooltip("Rotate 90° Right");
    }
    {
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImU32       col      = rightHovered ? hoverColor : normColor;
        drawList->AddTriangleFilled(ImVec2(cx + arrowDist + 6.0f * scale, cy), ImVec2(cx + arrowDist - 5.0f * scale, cy - 7.0f * scale), ImVec2(cx + arrowDist - 5.0f * scale, cy + 7.0f * scale), col);
    }
    ImGui::End();

    ImGui::PopStyleVar();

    // Track mouse interaction on the ViewCube to distinguish pure clicks from drags
    bool mouseInCube = ImGui::IsMouseHoveringRect(ImVec2(posX, posY), ImVec2(posX + cubeSize, posY + cubeSize));

    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && mouseInCube)
    {
        isCubeInteracting = true;
        isDraggingCube    = false;
        initialUp         = camera->getUp();
        mouseClickPos     = ImGui::GetMousePos();
    }

    if (isCubeInteracting && ImGui::IsMouseDown(ImGuiMouseButton_Left))
    {
        ImVec2 curPos   = ImGui::GetMousePos();
        float  dragDist = std::hypot(curPos.x - mouseClickPos.x, curPos.y - mouseClickPos.y);
        if (dragDist > 5.0f * scale)
        {
            isDraggingCube = true;
        }
    }

    if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
    {
        isCubeInteracting = false;
        isDraggingCube    = false;
    }

    // Render ViewCube itself
    auto      scene      = camera->getScene();
    glm::mat4 viewMatrix = scene.viewMatrix;
    glm::mat4 origMatrix = viewMatrix;

    ImGuizmo::SetDrawlist(ImGui::GetForegroundDrawList());
    ImGuizmo::ViewManipulate(glm::value_ptr(viewMatrix), static_cast<float>(dist), ImVec2(posX, posY), ImVec2(cubeSize, cubeSize), 0x10101010);

    if (viewMatrix != origMatrix)
    {
        // Suppress micro-drag while mouse is pressed down before reaching intentional drag threshold
        if (isCubeInteracting && !isDraggingCube)
        {
            return;
        }

        glm::mat4  invView    = glm::inverse(viewMatrix);
        glm::dvec3 newForward = glm::dvec3(invView[2]);

        if (glm::length(newForward) > 1e-6)
        {
            glm::dvec3 dir   = glm::normalize(newForward);
            glm::dvec3 refUp = (glm::length(initialUp) > 1e-4) ? initialUp : camera->getUp();
            glm::dvec3 newUp = getNearestAxisAlignedUp(dir, refUp);

            camera->setUp(newUp);
            camera->setEye(target + dir * dist);
        }
    }
    else
    {
        if (!isCubeInteracting)
        {
            initialUp = camera->getUp();
        }
    }
}

void ViewCubeWidget::rollCamera(Vulcant::Toolkit::Camera* camera, double angle)
{
    if (!camera)
    {
        return;
    }

    glm::dvec3 eye     = camera->getEye();
    glm::dvec3 target  = camera->getTarget();
    glm::dvec3 forward = target - eye;
    if (glm::length(forward) < 1e-6)
    {
        return;
    }
    forward = glm::normalize(forward);

    glm::dmat4 rot   = glm::rotate(glm::dmat4(1.0), angle, forward);
    glm::dvec3 newUp = glm::normalize(glm::dvec3(rot * glm::dvec4(camera->getUp(), 0.0)));

    camera->setUp(newUp);
    camera->setEye(eye);
}

void ViewCubeWidget::rotateAroundTarget(Vulcant::Toolkit::Camera* camera, double angle, const glm::dvec3& axis)
{
    if (!camera || glm::length(axis) < 1e-6)
    {
        return;
    }

    glm::dvec3 normAxis = glm::normalize(axis);
    glm::dvec3 eye      = camera->getEye();
    glm::dvec3 target   = camera->getTarget();
    glm::dvec3 toEye    = eye - target;
    if (glm::length(toEye) < 1e-6)
    {
        return;
    }

    glm::dmat4 rot      = glm::rotate(glm::dmat4(1.0), angle, normAxis);
    glm::dvec3 newToEye = glm::dvec3(rot * glm::dvec4(toEye, 0.0));
    glm::dvec3 newUp    = glm::normalize(glm::dvec3(rot * glm::dvec4(camera->getUp(), 0.0)));

    camera->setUp(newUp);
    camera->setEye(target + newToEye);
}

glm::dvec3 ViewCubeWidget::getNearestAxisAlignedUp(const glm::dvec3& dir, const glm::dvec3& currentUp) const
{
    const glm::dvec3 cardinalAxes[6] = { glm::dvec3(1.0, 0.0, 0.0),  glm::dvec3(-1.0, 0.0, 0.0), glm::dvec3(0.0, 1.0, 0.0),
                                         glm::dvec3(0.0, -1.0, 0.0), glm::dvec3(0.0, 0.0, 1.0),  glm::dvec3(0.0, 0.0, -1.0) };

    glm::dvec3 bestUp(0.0, 1.0, 0.0);
    double     maxDot = -2.0;

    for (int i = 0; i < 6; i++)
    {
        const glm::dvec3& axis = cardinalAxes[i];
        double            d    = glm::dot(axis, dir);
        if (std::abs(d) < 0.99)
        {
            glm::dvec3 proj      = glm::normalize(axis - d * dir);
            double     alignment = glm::dot(proj, currentUp);
            if (alignment > maxDot)
            {
                maxDot = alignment;
                bestUp = proj;
            }
        }
    }

    return bestUp;
}
