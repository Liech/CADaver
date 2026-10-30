#pragma once

#include <cstdint>
#include <memory>

class CadAsset;
class TriangulationAsset;
class VoxelAsset;

// Converts between CAD, Triangulation, and Voxel assets.
class AssetConverter
{
  public:
    AssetConverter();
    virtual ~AssetConverter();

    static std::shared_ptr<TriangulationAsset> cadToStl(const CadAsset& asset, double precision = 0.1);
    static std::shared_ptr<VoxelAsset>         cadToSvo(const CadAsset& asset, double precision = 0.1, uint32_t dimension = 64);
    static std::shared_ptr<VoxelAsset>         stlToSvo(const TriangulationAsset& asset, uint32_t dimension = 64);
    static std::shared_ptr<TriangulationAsset> svoToStlBlocky(const VoxelAsset& asset, int targetDepth = -1);
};
