#pragma once

#include <functional>
#include <glm/glm.hpp>

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

    float cubeSize = 120.0f;
    float margin   = 32.0f;
};
