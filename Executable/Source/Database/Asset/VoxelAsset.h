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
    VoxelAsset(const std::string& name);
    VoxelAsset(const std::string& name, std::unique_ptr<CADacomb::SVO<bool>> svo, const CADacomb::AABB3& aabb = CADacomb::AABB3());
    VoxelAsset(const std::string& name, const std::string& filePath, std::unique_ptr<CADacomb::SVO<bool>> svo, const CADacomb::AABB3& aabb = CADacomb::AABB3());
    virtual ~VoxelAsset();

    CADacomb::SVO<bool>* getSVO() const;
    void                 setSVO(std::unique_ptr<CADacomb::SVO<bool>> svo);

    const CADacomb::AABB3& getAABB() const;
    void                   setAABB(const CADacomb::AABB3& aabb);

    const std::string& getFilePath() const;
    void               setFilePath(const std::string& path);

    size_t getSizeInBytes() const override;

    bool isDirty() const;
    void markClean();

  private:
    std::unique_ptr<CADacomb::SVO<bool>> svo;
    CADacomb::AABB3                      aabb;
    std::string                          filePath;
    bool                                 dirty = true;
};
