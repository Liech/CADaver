#pragma once

#include "CADacomb/Texture/Texture.h"
#include "Database/Asset.h"

#include <glm/glm.hpp>
#include <memory>
#include <string>

// Headless data asset holding a 2D Texture placed in 3D space via its Asset transformation.
class TextureAsset : public Asset
{
  public:
    TextureAsset();
    virtual ~TextureAsset();

    std::string getType() const override;

    const CADacomb::Texture::Texture& getTexture() const;
    CADacomb::Texture::Texture&       getTexture();
    void                              setTexture(std::shared_ptr<CADacomb::Texture::Texture> texture);

    const glm::dvec2& getWorldSize() const;
    void              setWorldSize(const glm::dvec2& size);

    size_t getSizeInBytes() const override;

  private:
    std::shared_ptr<CADacomb::Texture::Texture> texture;
    glm::dvec2                                  worldSize = glm::dvec2(1.0, 1.0);
};
