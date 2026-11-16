#pragma once

#include <glm/glm.hpp>

// GPU uniform layout (std140) for CAD outline rendering of one asset. No virtuals: uploaded as raw memory.
struct AssetOutlineParams
{
    void setTransformation(const glm::dmat4& transformation);

    glm::vec4 lineColor{ 0.05f, 0.05f, 0.05f, 2.0f };
    glm::vec2 viewportSize{ 0.0f };
    glm::vec2 padding{ 0.0f };
    glm::mat4 modelMatrix{ 1.0f };
};
