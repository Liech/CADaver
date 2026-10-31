#include "Conversion/AssetConverter.h"

#include "CADacomb/CAD/Interface/TessellationOptions.h"
#include "CADacomb/CAD/Sm/KernelSm.h"
#include "CADacomb/Triangle/Triangulation.h"
#include "CADacomb/Util/ProgressBar.h"
#include "CADacomb/Volume/AABB3.h"
#include "CADacomb/Volume/SVO/Blocky/SVOBlockyTriangulation.h"
#include "CADacomb/Volume/SVO/SVO.h"
#include "CADacomb/Volume/SVO/SVOVoxelizer.h"
#include "Database/Asset/CadAsset.h"
#include "Database/Asset/TriangulationAsset.h"
#include "Database/Asset/VoxelAsset.h"

#include <algorithm>
#include <filesystem>
#include <glm/glm.hpp>
#include <memory>

AssetConverter::AssetConverter() {}

AssetConverter::~AssetConverter() {}

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
    return resultAsset;
}
