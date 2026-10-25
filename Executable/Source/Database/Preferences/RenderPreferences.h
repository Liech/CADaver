#pragma once

#include <glm/glm.hpp>
#include <nlohmann/json.hpp>
#include <string>

// Headless data model managing mesh color, wireframe styling, and face culling preferences.
class RenderPreferences
{
  public:
    RenderPreferences();
    virtual ~RenderPreferences();

    nlohmann::json toJson() const;
    void           fromJson(const nlohmann::json& jsonObject);

    const glm::vec3& getMeshColor() const;
    void             setMeshColor(const glm::vec3& color);

    int  getWireframeMode() const;
    void setWireframeMode(int mode);

    const glm::vec3& getWireframeColor() const;
    void             setWireframeColor(const glm::vec3& color);

    int  getCullMode() const;
    void setCullMode(int mode);

  private:
    glm::vec3 meshColor      = glm::vec3(0.7f, 0.75f, 0.85f);
    glm::vec3 wireframeColor = glm::vec3(1.0f, 0.0f, 0.0f);
    int       wireframeMode  = 0;
    int       cullMode       = 1;
};
