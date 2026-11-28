#include "Conversion/AssetConverter.h"

#include "CADacomb/CAD/Interface/TessellationOptions.h"
#include "CADacomb/CAD/Sm/KernelSm.h"
#include "CADacomb/SDF/SdfToSVOConverter.h"
#include "CADacomb/SDF/SdfToTriangulationConverter.h"
#include "CADacomb/Triangle/MeshSimplifier.h"
#include "CADacomb/Triangle/Triangulation.h"
#include "CADacomb/Util/AABB3.h"
#include "CADacomb/Util/ProgressBar.h"
#include "CADacomb/Volume/Flat/MarchingCubes.h"
#include "CADacomb/Volume/Flat/SurfaceNets.h"
#include "CADacomb/Volume/SVO/Blocky/SVOBlockyTriangulation.h"
#include "CADacomb/Volume/SVO/SVO.h"
#include "CADacomb/Volume/SVO/SVOVoxelizer.h"
#include "Database/Asset/CadAsset.h"
#include "Database/Asset/SdfAsset.h"
#include "Database/Asset/TriangulationAsset.h"
#include "Database/Asset/VoxelAsset.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <glm/glm.hpp>
#include <map>
#include <memory>
#include <tuple>
#include <vector>

AssetConverter::AssetConverter() {}

AssetConverter::~AssetConverter() {}

std::shared_ptr<VoxelAsset> AssetConverter::sdfToSvo(const SdfAsset& asset, uint32_t dimension)
{
    std::string error;
    auto        svo = CADacomb::SdfToSVOConverter::convert(asset.getSdf(), dimension, error);
    if (!svo)
    {
        return nullptr;
    }

    std::filesystem::path p(asset.getName());
    std::string           baseName = p.stem().string();
    if (baseName.empty())
    {
        baseName = asset.getName();
    }

    auto resultAsset = std::make_shared<VoxelAsset>();
    resultAsset->setName(baseName + ".cadacombsvo");
    resultAsset->setSVO(std::move(svo));
    resultAsset->setAABB(asset.getAABB());
    resultAsset->setTransformation(asset.getTransformation());
    return resultAsset;
}

std::shared_ptr<TriangulationAsset> AssetConverter::sdfToStl(const SdfAsset& asset, uint32_t resolution)
{
    std::string error;
    auto        triangulation = CADacomb::SdfToTriangulationConverter::convert(asset.getSdf(), resolution, error);
    if (!triangulation || triangulation->vertices.empty())
    {
        return nullptr;
    }

    std::filesystem::path p(asset.getName());
    std::string           baseName = p.stem().string();
    if (baseName.empty())
    {
        baseName = asset.getName();
    }

    auto resultAsset = std::make_shared<TriangulationAsset>();
    resultAsset->setName(baseName + ".stl");
    resultAsset->setTriangulation(std::move(triangulation));
    resultAsset->setTransformation(asset.getTransformation());
    return resultAsset;
}

std::shared_ptr<TriangulationAsset> AssetConverter::cadToStl(const CadAsset& asset, double precision)
{
    CADacomb::ProgressBar bar(1, "Triangulating CAD shape");

    CADacomb::CAD::KernelSm            kernel;
    CADacomb::CAD::TessellationOptions options;
    options.setChordHeightTolerance(precision);
    std::unique_ptr<CADacomb::Triangulation> newTri = kernel.tessellate(asset.getCadShape(), options);

    if ((!newTri || newTri->vertices.empty()) && !asset.getTriangulation().vertices.empty())
    {
        newTri           = std::make_unique<CADacomb::Triangulation>();
        newTri->vertices = asset.getTriangulation().vertices;
        newTri->indices  = asset.getTriangulation().indices;
    }

    if (!newTri || newTri->vertices.empty() || bar.isCanceled())
    {
        return nullptr;
    }

    bar.addProgress(1);

    std::filesystem::path p(asset.getName());
    std::string           baseName = p.stem().string();
    if (baseName.empty())
    {
        baseName = asset.getName();
    }
    std::string newName = baseName + ".stl";

    auto resultAsset = std::make_shared<TriangulationAsset>();
    resultAsset->setName(newName);
    resultAsset->setTriangulation(std::move(newTri));
    resultAsset->setTransformation(asset.getTransformation());
    return resultAsset;
}

std::shared_ptr<VoxelAsset> AssetConverter::cadToSvo(const CadAsset& asset, double precision, uint32_t dimension)
{
    auto stlAsset = cadToStl(asset, precision);
    if (!stlAsset)
    {
        return nullptr;
    }

    return stlToSvo(*stlAsset, dimension);
}

std::shared_ptr<VoxelAsset> AssetConverter::stlToSvo(const TriangulationAsset& asset, uint32_t dimension)
{
    CADacomb::ProgressBar bar(1, "Converting STL to SVO");

    const CADacomb::Triangulation& tri = asset.getTriangulation();
    if (tri.vertices.empty() || tri.indices.empty() || bar.isCanceled())
    {
        return nullptr;
    }

    auto   aabb   = tri.getAABB();
    double maxDim = std::max({ aabb.second.x, aabb.second.y, aabb.second.z });
    if (maxDim <= 0.0)
    {
        maxDim = 1.0;
    }

    double     margin   = maxDim * 0.02;
    glm::dvec3 start    = aabb.first - glm::dvec3(margin);
    double     cubeSize = maxDim + margin * 2.0;
    glm::dvec3 end      = start + glm::dvec3(cubeSize);

    CADacomb::SVOVoxelizer voxelizer;
    auto                   svo = voxelizer.voxelize<bool>(tri, start, end, dimension);
    if (!svo || bar.isCanceled())
    {
        return nullptr;
    }

    bar.addProgress(1);

    CADacomb::AABB3 bounds(start, end);

    std::filesystem::path p(asset.getName());
    std::string           baseName = p.stem().string();
    if (baseName.empty())
    {
        baseName = asset.getName();
    }
    std::string newName = baseName + ".cadacombsvo";

    auto resultAsset = std::make_shared<VoxelAsset>();
    resultAsset->setName(newName);
    resultAsset->setSVO(std::move(svo));
    resultAsset->setAABB(bounds);
    resultAsset->setTransformation(asset.getTransformation());
    return resultAsset;
}

std::shared_ptr<TriangulationAsset> AssetConverter::svoToStlBlocky(const VoxelAsset& asset, int targetDepth)
{
    CADacomb::ProgressBar bar(1, "Extracting blocky mesh from SVO");

    const CADacomb::SVO<bool>& svo = asset.getSVO();
    if (svo.getDimension() == 0 || bar.isCanceled())
    {
        return nullptr;
    }

    CADacomb::SVOBlockyOptions options;
    const auto&                aabb = asset.getAABB();
    options.origin                  = aabb.getMin();
    options.size                    = aabb.getSize();
    options.targetDepth             = targetDepth;

    auto triangulation = CADacomb::SVOBlockyTriangulation::triangulate(svo, options);
    if (!triangulation || bar.isCanceled())
    {
        return nullptr;
    }

    bar.addProgress(1);

    std::filesystem::path p(asset.getName());
    std::string           baseName = p.stem().string();
    if (baseName.empty())
    {
        baseName = asset.getName();
    }
    std::string newName = baseName + "_blocky.stl";

    auto resultAsset = std::make_shared<TriangulationAsset>();
    resultAsset->setName(newName);
    resultAsset->setTriangulation(std::move(triangulation));
    resultAsset->setTransformation(asset.getTransformation());
    return resultAsset;
}

std::shared_ptr<TriangulationAsset> AssetConverter::svoToStlSurfaceNets(const VoxelAsset& asset, int targetDepth)
{
    CADacomb::ProgressBar bar(2, "Extracting SurfaceNets mesh from SVO");

    const CADacomb::SVO<bool>& svo       = asset.getSVO();
    uint32_t                   dimension = svo.getDimension();
    if (dimension == 0 || bar.isCanceled())
    {
        return nullptr;
    }

    int      maxDepth = 0;
    uint32_t tempDim  = dimension;
    while (tempDim >>= 1)
    {
        maxDepth++;
    }

    int effectiveDepth = targetDepth;
    if (effectiveDepth < 0 || effectiveDepth > maxDepth)
    {
        effectiveDepth = maxDepth;
    }

    uint32_t          gridDim = 1u << effectiveDepth;
    std::vector<bool> rawGrid;
    svo.get3DArray(rawGrid, glm::ivec3(0), glm::ivec3(static_cast<int>(dimension)), effectiveDepth);
    bar.addProgress(1);
    if (bar.isCanceled())
    {
        return nullptr;
    }

    size_t            padDim = static_cast<size_t>(gridDim) + 2;
    std::vector<bool> padded(padDim * padDim * padDim, false);
    for (size_t z = 0; z < gridDim; z++)
    {
        for (size_t y = 0; y < gridDim; y++)
        {
            for (size_t x = 0; x < gridDim; x++)
            {
                if (rawGrid[x + y * gridDim + z * gridDim * gridDim])
                {
                    padded[(x + 1) + (y + 1) * padDim + (z + 1) * padDim * padDim] = true;
                }
            }
        }
    }

    const auto&  aabb      = asset.getAABB();
    glm::dvec3   voxelSize = aabb.getSize() / static_cast<double>(gridDim);
    glm::dvec3   padOrigin = aabb.getMin() - 0.5 * voxelSize;
    glm::u64vec3 padRes(padDim, padDim, padDim);

    auto triangulation = CADacomb::SurfaceNets::triangulate(padded, padOrigin, voxelSize, padRes);
    if (!triangulation || triangulation->vertices.empty() || bar.isCanceled())
    {
        return nullptr;
    }
    bar.addProgress(1);

    std::filesystem::path p(asset.getName());
    std::string           baseName = p.stem().string();
    if (baseName.empty())
    {
        baseName = asset.getName();
    }
    std::string newName = baseName + "_surfacenets.stl";

    auto resultAsset = std::make_shared<TriangulationAsset>();
    resultAsset->setName(newName);
    resultAsset->setTriangulation(std::move(triangulation));
    resultAsset->setTransformation(asset.getTransformation());
    return resultAsset;
}

std::shared_ptr<TriangulationAsset> AssetConverter::svoToStlMarchingCubes(const VoxelAsset& asset, int targetDepth)
{
    CADacomb::ProgressBar bar(2, "Extracting Marching Cubes mesh from SVO");

    const CADacomb::SVO<bool>& svo       = asset.getSVO();
    uint32_t                   dimension = svo.getDimension();
    if (dimension == 0 || bar.isCanceled())
    {
        return nullptr;
    }

    int      maxDepth = 0;
    uint32_t tempDim  = dimension;
    while (tempDim >>= 1)
    {
        maxDepth++;
    }

    int effectiveDepth = targetDepth;
    if (effectiveDepth < 0 || effectiveDepth > maxDepth)
    {
        effectiveDepth = maxDepth;
    }

    uint32_t          gridDim = 1u << effectiveDepth;
    std::vector<bool> rawGrid;
    svo.get3DArray(rawGrid, glm::ivec3(0), glm::ivec3(static_cast<int>(dimension)), effectiveDepth);
    bar.addProgress(1);
    if (bar.isCanceled())
    {
        return nullptr;
    }

    size_t              padDim = static_cast<size_t>(gridDim) + 2;
    std::vector<double> padded(padDim * padDim * padDim, 0.0);
    for (size_t z = 0; z < gridDim; z++)
    {
        for (size_t y = 0; y < gridDim; y++)
        {
            for (size_t x = 0; x < gridDim; x++)
            {
                if (rawGrid[x + y * gridDim + z * gridDim * gridDim])
                {
                    padded[(x + 1) + (y + 1) * padDim + (z + 1) * padDim * padDim] = 1.0;
                }
            }
        }
    }

    const auto&             aabb      = asset.getAABB();
    glm::dvec3              voxelSize = aabb.getSize() / static_cast<double>(gridDim);
    glm::dvec3              padOrigin = aabb.getMin() - 0.5 * voxelSize;
    glm::u64vec3            padRes(padDim, padDim, padDim);
    std::vector<glm::dvec3> soup = CADacomb::MarchingCubes::polygonize(padded, padOrigin, voxelSize, padRes, 0.5);
    if (soup.empty() || bar.isCanceled())
    {
        return nullptr;
    }

    auto   triangulation = std::make_unique<CADacomb::Triangulation>();
    double diag          = glm::length(aabb.getSize());
    double eps           = std::max(diag * 1e-9, 1e-12);
    double inv           = 1.0 / eps;

    std::map<std::tuple<int64_t, int64_t, int64_t>, size_t> vertMap;
    triangulation->indices.reserve(soup.size());
    for (const glm::dvec3& v : soup)
    {
        std::tuple<int64_t, int64_t, int64_t> key(static_cast<int64_t>(std::llround(v.x * inv)), static_cast<int64_t>(std::llround(v.y * inv)), static_cast<int64_t>(std::llround(v.z * inv)));
        auto                                  it = vertMap.find(key);
        if (it == vertMap.end())
        {
            size_t idx   = triangulation->vertices.size();
            vertMap[key] = idx;
            triangulation->vertices.push_back(v);
            triangulation->indices.push_back(idx);
        }
        else
        {
            triangulation->indices.push_back(it->second);
        }
    }
    bar.addProgress(1);

    std::filesystem::path p(asset.getName());
    std::string           baseName = p.stem().string();
    if (baseName.empty())
    {
        baseName = asset.getName();
    }
    std::string newName = baseName + "_marchingcubes.stl";

    auto resultAsset = std::make_shared<TriangulationAsset>();
    resultAsset->setName(newName);
    resultAsset->setTriangulation(std::move(triangulation));
    resultAsset->setTransformation(asset.getTransformation());
    return resultAsset;
}

std::shared_ptr<TriangulationAsset> AssetConverter::simplifyMeshByPercent(const TriangulationAsset& asset, double targetPercent)
{
    const CADacomb::Triangulation& tri = asset.getTriangulation();
    if (tri.vertices.empty() || tri.indices.empty())
    {
        return nullptr;
    }

    auto simplified = CADacomb::MeshSimplifier::simplifyByPercent(tri, targetPercent);
    if (!simplified || simplified->vertices.empty() || simplified->indices.empty())
    {
        return nullptr;
    }

    std::filesystem::path p(asset.getName());
    std::string           baseName = p.stem().string();
    if (baseName.empty())
    {
        baseName = asset.getName();
    }
    std::string newName = baseName + "_simplified.stl";

    auto resultAsset = std::make_shared<TriangulationAsset>();
    resultAsset->setName(newName);
    resultAsset->setTriangulation(std::move(simplified));
    resultAsset->setTransformation(asset.getTransformation());
    return resultAsset;
}

std::shared_ptr<TriangulationAsset> AssetConverter::simplifyMeshByTriangleCount(const TriangulationAsset& asset, size_t targetTriangleCount)
{
    const CADacomb::Triangulation& tri = asset.getTriangulation();
    if (tri.vertices.empty() || tri.indices.empty())
    {
        return nullptr;
    }

    auto simplified = CADacomb::MeshSimplifier::simplifyByTriangleCount(tri, targetTriangleCount);
    if (!simplified || simplified->vertices.empty() || simplified->indices.empty())
    {
        return nullptr;
    }

    std::filesystem::path p(asset.getName());
    std::string           baseName = p.stem().string();
    if (baseName.empty())
    {
        baseName = asset.getName();
    }
    std::string newName = baseName + "_simplified.stl";

    auto resultAsset = std::make_shared<TriangulationAsset>();
    resultAsset->setName(newName);
    resultAsset->setTriangulation(std::move(simplified));
    resultAsset->setTransformation(asset.getTransformation());
    return resultAsset;
}
