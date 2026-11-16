#include "Rendering/AssetMeshParams.h"

void AssetMeshParams::setTransformation(const glm::dmat4& transformation)
{
    modelMatrix  = glm::mat4(transformation);
    normalMatrix = glm::transpose(glm::inverse(modelMatrix));
}
