#pragma once

#include "Vulcant/Toolkit/MeshShaderParams.h"

#include <glm/glm.hpp>

// GPU uniform layout (std140) for mesh rendering of one asset: toolkit mesh params plus the asset's model and normal matrices.
struct AssetMeshParams
{
    void setTransformation(const glm::dmat4& transformation);

    Vulcant::Toolkit::MeshShaderParams params;
    glm::mat4                          modelMatrix{ 1.0f };
    glm::mat4                          normalMatrix{ 1.0f };
};
