#pragma once

#include <cstdint>
#include <memory>

class CadAsset;
class SdfAsset;
class TriangulationAsset;
class VoxelAsset;

// Converts between CAD, SDF, Triangulation, and Voxel assets.
class AssetConverter
{
  public:
    AssetConverter();
    virtual ~AssetConverter();

    static std::shared_ptr<TriangulationAsset> cadToStl(const CadAsset& asset, double precision = 0.1);
    static std::shared_ptr<VoxelAsset>         cadToSvo(const CadAsset& asset, double precision = 0.1, uint32_t dimension = 64);
    static std::shared_ptr<VoxelAsset>         sdfToSvo(const SdfAsset& asset, uint32_t dimension = 64);
    static std::shared_ptr<TriangulationAsset> sdfToStl(const SdfAsset& asset, uint32_t resolution = 64);
    static std::shared_ptr<VoxelAsset>         stlToSvo(const TriangulationAsset& asset, uint32_t dimension = 64);
    static std::shared_ptr<TriangulationAsset> svoToStlBlocky(const VoxelAsset& asset, int targetDepth = -1);
    static std::shared_ptr<TriangulationAsset> svoToStlSurfaceNets(const VoxelAsset& asset, int targetDepth = -1);
    static std::shared_ptr<TriangulationAsset> svoToStlMarchingCubes(const VoxelAsset& asset, int targetDepth = -1);
    static std::shared_ptr<TriangulationAsset> simplifyMeshByPercent(const TriangulationAsset& asset, double targetPercent);
    static std::shared_ptr<TriangulationAsset> simplifyMeshByTriangleCount(const TriangulationAsset& asset, size_t targetTriangleCount);
};
