#pragma once

#include "CADacomb/Animation/Skinning/TriangulationSkinning.h"
#include "Database/AssetComponent.h"

#include <string>

// Headless AssetComponent attaching per-vertex skeletal skinning weights to a TriangulationAsset.
class TriangulationSkinningComponent : public AssetComponent
{
  public:
    TriangulationSkinningComponent();
    virtual ~TriangulationSkinningComponent();

    std::string getType() const override;
    std::string getDescription() const override;
    size_t      getSizeInBytes() const override;
    bool        isCompatible(const std::string& assetType) const override;

    nlohmann::json toDataJson() const override;
    void           fromDataJson(const nlohmann::json& jsonObject) override;

    const CADacomb::Animation::TriangulationSkinning& getSkinning() const;
    CADacomb::Animation::TriangulationSkinning&       getSkinning();
    void                                              setSkinning(const CADacomb::Animation::TriangulationSkinning& skinning);

  private:
    CADacomb::Animation::TriangulationSkinning skinning;
};
