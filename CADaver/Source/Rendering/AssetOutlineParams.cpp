#include "Rendering/AssetOutlineParams.h"

void AssetOutlineParams::setTransformation(const glm::dmat4& transformation)
{
    modelMatrix = glm::mat4(transformation);
}
