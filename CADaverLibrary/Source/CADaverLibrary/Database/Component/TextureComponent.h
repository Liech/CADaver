#pragma once

#include "CADacomb/Texture/Texture.h"
#include "Database/AssetComponent.h"

#include <memory>
#include <string>

// Headless AssetComponent attaching a 2D Texture to a TriangulationAsset.
class TextureComponent : public AssetComponent
{
  public:
    TextureComponent();
    virtual ~TextureComponent();

    std::string getType() const override;
    std::string getDescription() const override;
    size_t      getSizeInBytes() const override;
    bool        isCompatible(const std::string& assetType) const override;

    nlohmann::json toDataJson() const override;
    void           fromDataJson(const nlohmann::json& jsonObject) override;

    const CADacomb::Texture::Texture& getTexture() const;
    CADacomb::Texture::Texture&       getTexture();
    void                              setTexture(std::shared_ptr<CADacomb::Texture::Texture> texture);

  private:
    std::shared_ptr<CADacomb::Texture::Texture> texture;
};
