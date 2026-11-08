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

    void render(Vulcant::Toolkit::Camera* camera, const glm::ivec2& resolution, const std::function<void()>& onHomePressed, const std::function<void(bool)>& onOrthographicChanged = nullptr);

  private:
    void rollCamera(Vulcant::Toolkit::Camera* camera, double angle);
    void rotateAroundTarget(Vulcant::Toolkit::Camera* camera, double angle, const glm::dvec3& axis);

    float baseCubeSize  = 110.0f;
    float baseMargin    = 28.0f;
    float baseArrowDist = 54.0f;
};
