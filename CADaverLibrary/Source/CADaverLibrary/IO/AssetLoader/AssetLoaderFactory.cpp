#include "IO/AssetLoader/AssetLoaderFactory.h"

#include "IO/AssetLoader/CadAssetLoader.h"
#include "IO/AssetLoader/TriangulationAssetLoader.h"
#include "IO/AssetLoader/VoxelAssetLoader.h"
#include "Utility/IOValueGrabber.h"

#include <algorithm>
#include <cctype>
#include <filesystem>

AssetLoaderFactory::AssetLoaderFactory()
{
    registerLoaders();
}

AssetLoaderFactory::~AssetLoaderFactory() {}

std::unique_ptr<AssetLoader> AssetLoaderFactory::create(const std::string& assetType) const
{
    auto it = creators.find(assetType);
    if (it != creators.end() && it->second)
    {
        return it->second();
    }

    return nullptr;
}

std::string AssetLoaderFactory::resolveAssetType(const std::string& assetType, const Polyglot::Node& source, const std::string& name) const
{
    if (!assetType.empty())
    {
        if (creators.find(assetType) != creators.end())
        {
            return assetType;
        }

        std::string lowerType = assetType;
        std::transform(lowerType.begin(), lowerType.end(), lowerType.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        auto extIt = extensionToType.find(lowerType);
        if (extIt != extensionToType.end())
        {
            return extIt->second;
        }
        return assetType;
    }

    if (IOValueGrabber::isFilePath(source))
    {
        std::string ext   = extractExtension(IOValueGrabber::grabFilePath(source));
        auto        extIt = extensionToType.find(ext);
        if (extIt != extensionToType.end())
        {
            return extIt->second;
        }
    }

    std::string nameExt = extractExtension(name);
    auto        nameIt  = extensionToType.find(nameExt);
    if (nameIt != extensionToType.end())
    {
        return nameIt->second;
    }

    return "";
}

void AssetLoaderFactory::registerLoaders()
{
    creators["Cad"]           = []() { return std::make_unique<CadAssetLoader>(); };
    creators["Triangulation"] = []() { return std::make_unique<TriangulationAssetLoader>(); };
    creators["Voxel"]         = []() { return std::make_unique<VoxelAssetLoader>(); };

    extensionToType["step"]        = "Cad";
    extensionToType["stp"]         = "Cad";
    extensionToType["cad"]         = "Cad";
    extensionToType["stl"]         = "Triangulation";
    extensionToType["triangulation"] = "Triangulation";
    extensionToType["cadacombsvo"] = "Voxel";
    extensionToType["svo"]         = "Voxel";
    extensionToType["voxel"]       = "Voxel";
}

std::string AssetLoaderFactory::extractExtension(const std::string& pathStr) const
{
    std::string ext = std::filesystem::path(pathStr).extension().string();
    if (!ext.empty() && ext.front() == '.')
    {
        ext.erase(0, 1);
    }
    std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return ext;
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("AssetLoaderFactory/CreateAndResolve")
{
    AssetLoaderFactory factory;
    CHECK(factory.create("Cad") != nullptr);
    CHECK(factory.create("Triangulation") != nullptr);
    CHECK(factory.create("Voxel") != nullptr);
    CHECK(factory.create("Unknown") == nullptr);

    CHECK(factory.resolveAssetType("Cad", Polyglot::Node("dummy"), "model") == "Cad");
    CHECK(factory.resolveAssetType("", Polyglot::Node("part.step"), "Part") == "Cad");
    CHECK(factory.resolveAssetType("", Polyglot::Node("mesh.STL"), "Mesh") == "Triangulation");
    CHECK(factory.resolveAssetType("", Polyglot::Node("vol.cadacombsvo"), "Vol") == "Voxel");
}
#endif
