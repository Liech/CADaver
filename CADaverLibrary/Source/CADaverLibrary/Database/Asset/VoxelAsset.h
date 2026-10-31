#pragma once

#include "CADacomb/Volume/AABB3.h"
#include "CADacomb/Volume/SVO/SVO.h"
#include "Database/Asset.h"

#include <memory>
#include <string>

// Headless data asset representing volumetric voxel geometry.
class VoxelAsset : public Asset
{
  public:
    VoxelAsset();
    virtual ~VoxelAsset();

    std::string getType() const override;

    const CADacomb::SVO<bool>* getSVO() const;
    CADacomb::SVO<bool>*       getSVO();
    void                       setSVO(std::unique_ptr<CADacomb::SVO<bool>> svo);

    const CADacomb::AABB3& getAABB() const;
    void                   setAABB(const CADacomb::AABB3& aabb);

    size_t getSizeInBytes() const override;

  private:
    std::unique_ptr<CADacomb::SVO<bool>> svo;
    CADacomb::AABB3                      aabb;
};
