#pragma once

#include "CADacomb/Animation/KeyframeAnimation/AnimationClip.h"
#include "Database/AssetComponent.h"

#include <cstddef>
#include <string>
#include <vector>

// Headless AssetComponent attaching keyframe AnimationClips and playback settings to a TriangulationAsset.
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

    const std::vector<CADacomb::Animation::AnimationClip>& getClips() const;
    std::vector<CADacomb::Animation::AnimationClip>&       getClips();
    void                                                   addClip(const CADacomb::Animation::AnimationClip& clip);
    void                                                   setClips(const std::vector<CADacomb::Animation::AnimationClip>& clips);

    size_t getActiveClipIndex() const;
    void   setActiveClipIndex(size_t index);

    bool isLooping() const;
    void setLooping(bool looping);

  private:
    std::vector<CADacomb::Animation::AnimationClip> clips;
    size_t                                          activeClipIndex = 0;
    bool                                            looping         = true;
};
