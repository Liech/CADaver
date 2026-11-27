#pragma once

#include "CADacomb/Animation/Skeleton/Skeleton.h"
#include "Database/AssetComponent.h"

#include <string>

// Headless AssetComponent attaching a skeletal joint hierarchy to a TriangulationAsset.
class SkeletonComponent : public AssetComponent
{
  public:
    SkeletonComponent();
    virtual ~SkeletonComponent();

    std::string getType() const override;
    std::string getDescription() const override;
    size_t      getSizeInBytes() const override;
    bool        isCompatible(const std::string& assetType) const override;

    nlohmann::json toDataJson() const override;
    void           fromDataJson(const nlohmann::json& jsonObject) override;

    const CADacomb::Animation::Skeleton& getSkeleton() const;
    CADacomb::Animation::Skeleton&       getSkeleton();
    void                                 setSkeleton(const CADacomb::Animation::Skeleton& skeleton);

  private:
    CADacomb::Animation::Skeleton skeleton;
};
