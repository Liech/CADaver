#pragma once

#include "CADacomb/Texture/Mapping/TriangulationUVs.h"
#include "Database/AssetComponent.h"

#include <string>

// Headless AssetComponent attaching per-face-corner UV coordinates to a TriangulationAsset.
class UVComponent : public AssetComponent
{
  public:
    UVComponent();
    virtual ~UVComponent();

    std::string getType() const override;
    std::string getDescription() const override;
    size_t      getSizeInBytes() const override;
    bool        isCompatible(const std::string& assetType) const override;

    nlohmann::json toDataJson() const override;
    void           fromDataJson(const nlohmann::json& jsonObject) override;

    const CADacomb::Texture::TriangulationUVs& getUVs() const;
    CADacomb::Texture::TriangulationUVs&       getUVs();
    void                                       setUVs(const CADacomb::Texture::TriangulationUVs& uvs);

  private:
    CADacomb::Texture::TriangulationUVs uvs;
};
