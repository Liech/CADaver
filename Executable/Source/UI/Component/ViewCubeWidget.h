#pragma once

#include <functional>
#include <glm/glm.hpp>
#include <imgui.h>

namespace Vulcant::Toolkit
{
    class Camera;
}

// Interactive ViewCube, 90 degree rotation controls, and Home button for viewport navigation.
class ViewCubeWidget
{
  public:
    ViewCubeWidget();
    virtual ~ViewCubeWidget();

    void render(Vulcant::Toolkit::Camera* camera, const glm::ivec2& resolution, const std::function<void()>& onHomePressed);

  private:
    void       rollCamera(Vulcant::Toolkit::Camera* camera, double angle);
    void       rotateAroundTarget(Vulcant::Toolkit::Camera* camera, double angle, const glm::dvec3& axis);
    glm::dvec3 getNearestAxisAlignedUp(const glm::dvec3& dir, const glm::dvec3& currentUp) const;

    float baseCubeSize  = 110.0f;
    float baseMargin    = 28.0f;
    float baseArrowDist = 54.0f;

    bool       isCubeInteracting = false;
    bool       isDraggingCube    = false;
    ImVec2     mouseClickPos     = ImVec2(0.0f, 0.0f);
    glm::dvec3 initialUp         = glm::dvec3(0.0, 1.0, 0.0);
};
