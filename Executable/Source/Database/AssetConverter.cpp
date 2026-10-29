#include "AssetConverter.h"

#include "Asset/CadAsset.h"
#include "Asset/TriangulationAsset.h"
#include "Asset/VoxelAsset.h"
#include "CADacomb/Triangle/Triangulation.h"
#include "CADacomb/Util/ProgressBar.h"
#include "CADacomb/Volume/AABB3.h"
#include "CADacomb/Volume/SVO/Blocky/SVOBlockyTriangulation.h"
#include "CADacomb/Volume/SVO/SVO.h"
#include "CADacomb/Volume/SVO/SVOVoxelizer.h"
#include "Library/Operation/TriangulateOperation.h"

#include <algorithm>
#include <filesystem>
#include <glm/glm.hpp>
#include <memory>

AssetConverter::AssetConverter() {}

AssetConverter::~AssetConverter() {}

std::shared_ptr<TriangulationAsset> AssetConverter::cadToStl(const CadAsset& asset, double precision)
{
    CADacomb::ProgressBar bar(1, "Triangulating CAD shape");

    std::unique_ptr<CADacomb::Triangulation> newTri;

    if (asset.getCadShape())
    {
        newTri = Library::TriangulateOperation::triangulate(*asset.getCadShape(), precision);
    }
    else if (const CADacomb::Triangulation* tri = asset.getTriangulation())
    {
        if (!tri->vertices.empty())
        {
            newTri           = std::make_unique<CADacomb::Triangulation>();
            newTri->vertices = tri->vertices;
            newTri->indices  = tri->indices;
        }
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

    return std::make_shared<TriangulationAsset>(newName, std::move(newTri));
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

    const CADacomb::Triangulation* tri = asset.getTriangulation();
    if (!tri || tri->vertices.empty() || tri->indices.empty() || bar.isCanceled())
    {
        return nullptr;
    }

    auto   aabb   = tri->getAABB();
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
    auto                   svo = voxelizer.voxelize<bool>(*tri, start, end, dimension);
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

    return std::make_shared<VoxelAsset>(newName, "", std::move(svo), bounds);
}

std::shared_ptr<TriangulationAsset> AssetConverter::svoToStlBlocky(const VoxelAsset& asset, int targetDepth)
{
    CADacomb::ProgressBar bar(1, "Extracting blocky mesh from SVO");

    const CADacomb::SVO<bool>* svo = asset.getSVO();
    if (!svo || svo->getDimension() == 0 || bar.isCanceled())
    {
        return nullptr;
    }

    CADacomb::SVOBlockyOptions options;
    const auto&                aabb = asset.getAABB();
    options.origin                  = aabb.getMin();
    options.size                    = aabb.getSize();
    options.targetDepth             = targetDepth;

    auto triangulation = CADacomb::SVOBlockyTriangulation::triangulate(*svo, options);
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

    return std::make_shared<TriangulationAsset>(newName, std::move(triangulation));
}
