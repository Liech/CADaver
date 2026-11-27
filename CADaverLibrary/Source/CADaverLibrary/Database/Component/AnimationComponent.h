#pragma once

#include "CADacomb/Animation/KeyframeAnimation/AnimationClip.h"
#include "Database/AssetComponent.h"

#include <string>

// Headless AssetComponent attaching a single keyframe AnimationClip to a TriangulationAsset.
class AnimationComponent : public AssetComponent
{
  public:
    AnimationComponent();
    virtual ~AnimationComponent();

    std::string getType() const override;
    std::string getDescription() const override;
    size_t      getSizeInBytes() const override;
    bool        isCompatible(const std::string& assetType) const override;

    nlohmann::json toDataJson() const override;
    void           fromDataJson(const nlohmann::json& jsonObject) override;

    const CADacomb::Animation::AnimationClip& getClip() const;
    CADacomb::Animation::AnimationClip&       getClip();
    void                                      setClip(const CADacomb::Animation::AnimationClip& clip);

    bool isLooping() const;
    void setLooping(bool looping);

  private:
    CADacomb::Animation::AnimationClip clip;
    bool                               looping = true;
};
