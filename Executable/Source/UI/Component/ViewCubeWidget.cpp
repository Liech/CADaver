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

    float menuBarHeight = ImGui::GetFrameHeight();
    float posX          = static_cast<float>(resolution.x) - cubeSize - margin;
    float posY          = menuBarHeight + margin;
    float cx            = posX + cubeSize * 0.5f;
    float cy            = posY + cubeSize * 0.5f;
    float arrowDist     = 58.0f;

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
    ImGui::SetNextWindowPos(ImVec2(cx - 62.0f - 12.0f, cy - arrowDist - 12.0f));
    ImGui::Begin("##ViewCubeHome", nullptr, windowFlags);
    {
        bool hasEmojiGlyph = (sizeof(ImWchar) >= 4 && ImGui::GetFont() && ImGui::GetFont()->IsGlyphInFont(static_cast<ImWchar>(0x1F3E0)));
        bool homeClicked   = false;
        if (hasEmojiGlyph)
        {
            homeClicked = ImGui::Button(reinterpret_cast<const char*>(u8"🏠"), ImVec2(24.0f, 24.0f));
        }
        else
        {
            homeClicked = ImGui::Button("##HomeButton", ImVec2(24.0f, 24.0f));

            ImDrawList* drawList = ImGui::GetWindowDrawList();
            ImVec2      btnMin   = ImGui::GetItemRectMin();
            ImVec2      btnMax   = ImGui::GetItemRectMax();
            ImVec2      center   = ImVec2((btnMin.x + btnMax.x) * 0.5f, (btnMin.y + btnMax.y) * 0.5f);

            ImU32 iconColor = ImGui::GetColorU32(ImGuiCol_Text);
            ImU32 doorColor = ImGui::GetColorU32(ImGuiCol_Button);

            ImVec2 pRoofTop(center.x, center.y - 7.0f);
            ImVec2 pRoofLeft(center.x - 7.0f, center.y - 1.0f);
            ImVec2 pRoofRight(center.x + 7.0f, center.y - 1.0f);
            drawList->AddTriangleFilled(pRoofTop, pRoofLeft, pRoofRight, iconColor);
            drawList->AddRectFilled(ImVec2(center.x + 2.5f, center.y - 6.0f), ImVec2(center.x + 4.5f, center.y - 2.0f), iconColor);
            drawList->AddRectFilled(ImVec2(center.x - 5.0f, center.y - 1.0f), ImVec2(center.x + 5.0f, center.y + 6.0f), iconColor);
            drawList->AddRectFilled(ImVec2(center.x - 1.5f, center.y + 2.0f), ImVec2(center.x + 1.5f, center.y + 6.0f), doorColor);
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
    ImGui::SetNextWindowPos(ImVec2(cx - 36.0f - 10.0f, cy - arrowDist - 10.0f));
    ImGui::Begin("##ViewCubeRollCCW", nullptr, windowFlags);
    if (ImGui::InvisibleButton("##RollCCW", ImVec2(20.0f, 20.0f)))
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
        ImVec2      arcC     = ImVec2(cx - 36.0f, cy - arrowDist);
        drawList->PathArcTo(arcC, 6.0f, glm::radians(140.0f), glm::radians(330.0f), 8);
        drawList->PathStroke(col, 0, 2.0f);
        drawList->AddTriangleFilled(ImVec2(arcC.x - 7.0f, arcC.y - 1.0f), ImVec2(arcC.x - 2.0f, arcC.y - 6.0f), ImVec2(arcC.x - 1.0f, arcC.y), col);
    }
    ImGui::End();

    // Roll CW button
    ImGui::SetNextWindowPos(ImVec2(cx + 36.0f - 10.0f, cy - arrowDist - 10.0f));
    ImGui::Begin("##ViewCubeRollCW", nullptr, windowFlags);
    if (ImGui::InvisibleButton("##RollCW", ImVec2(20.0f, 20.0f)))
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
        ImVec2      arcC     = ImVec2(cx + 36.0f, cy - arrowDist);
        drawList->PathArcTo(arcC, 6.0f, glm::radians(210.0f), glm::radians(400.0f), 8);
        drawList->PathStroke(col, 0, 2.0f);
        drawList->AddTriangleFilled(ImVec2(arcC.x + 7.0f, arcC.y - 1.0f), ImVec2(arcC.x + 2.0f, arcC.y - 6.0f), ImVec2(arcC.x + 1.0f, arcC.y), col);
    }
    ImGui::End();

    // Up arrow button
    ImGui::SetNextWindowPos(ImVec2(cx - 10.0f, cy - arrowDist - 8.0f));
    ImGui::Begin("##ViewCubeArrowUp", nullptr, windowFlags);
    if (ImGui::InvisibleButton("##ArrowUp", ImVec2(20.0f, 16.0f)))
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
        drawList->AddTriangleFilled(ImVec2(cx, cy - arrowDist - 6.0f), ImVec2(cx - 7.0f, cy - arrowDist + 5.0f), ImVec2(cx + 7.0f, cy - arrowDist + 5.0f), col);
    }
    ImGui::End();

    // Down arrow button
    ImGui::SetNextWindowPos(ImVec2(cx - 10.0f, cy + arrowDist - 8.0f));
    ImGui::Begin("##ViewCubeArrowDown", nullptr, windowFlags);
    if (ImGui::InvisibleButton("##ArrowDown", ImVec2(20.0f, 16.0f)))
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
        drawList->AddTriangleFilled(ImVec2(cx, cy + arrowDist + 6.0f), ImVec2(cx - 7.0f, cy + arrowDist - 5.0f), ImVec2(cx + 7.0f, cy + arrowDist - 5.0f), col);
    }
    ImGui::End();

    // Left arrow button
    ImGui::SetNextWindowPos(ImVec2(cx - arrowDist - 8.0f, cy - 10.0f));
    ImGui::Begin("##ViewCubeArrowLeft", nullptr, windowFlags);
    if (ImGui::InvisibleButton("##ArrowLeft", ImVec2(16.0f, 20.0f)))
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
        drawList->AddTriangleFilled(ImVec2(cx - arrowDist - 6.0f, cy), ImVec2(cx - arrowDist + 5.0f, cy - 7.0f), ImVec2(cx - arrowDist + 5.0f, cy + 7.0f), col);
    }
    ImGui::End();

    // Right arrow button
    ImGui::SetNextWindowPos(ImVec2(cx + arrowDist - 8.0f, cy - 10.0f));
    ImGui::Begin("##ViewCubeArrowRight", nullptr, windowFlags);
    if (ImGui::InvisibleButton("##ArrowRight", ImVec2(16.0f, 20.0f)))
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
        drawList->AddTriangleFilled(ImVec2(cx + arrowDist + 6.0f, cy), ImVec2(cx + arrowDist - 5.0f, cy - 7.0f), ImVec2(cx + arrowDist - 5.0f, cy + 7.0f), col);
    }
    ImGui::End();

    ImGui::PopStyleVar();

    // Render ViewCube itself
    auto      scene      = camera->getScene();
    glm::mat4 viewMatrix = scene.viewMatrix;
    glm::mat4 origMatrix = viewMatrix;

    ImGuizmo::SetDrawlist(ImGui::GetForegroundDrawList());
    ImGuizmo::ViewManipulate(glm::value_ptr(viewMatrix), static_cast<float>(dist), ImVec2(posX, posY), ImVec2(cubeSize, cubeSize), 0x10101010);

    if (viewMatrix != origMatrix)
    {
        glm::mat4  invView    = glm::inverse(viewMatrix);
        glm::dvec3 newForward = glm::dvec3(invView[2]);

        if (glm::length(newForward) > 1e-6)
        {
            glm::dvec3 dir   = glm::normalize(newForward);
            glm::dvec3 oldUp = camera->getUp();
            glm::dvec3 newUp = getNearestAxisAlignedUp(dir, oldUp);

            camera->setUp(newUp);
            camera->setEye(target + dir * dist);
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
