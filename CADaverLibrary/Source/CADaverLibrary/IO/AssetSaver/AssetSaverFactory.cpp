#include "IO/AssetSaver/AssetSaverFactory.h"

#include "IO/AssetSaver/CadAssetSaver.h"
#include "IO/AssetSaver/SdfAssetSaver.h"
#include "IO/AssetSaver/TextureAssetSaver.h"
#include "IO/AssetSaver/TriangulationAssetSaver.h"
#include "IO/AssetSaver/VectorGraphicsAssetSaver.h"
#include "IO/AssetSaver/VoxelAssetSaver.h"

AssetSaverFactory::AssetSaverFactory()
{
    registerSavers();
}

AssetSaverFactory::~AssetSaverFactory() {}

std::unique_ptr<AssetSaver> AssetSaverFactory::create(const std::string& assetType) const
{
    auto it = creators.find(assetType);
    if (it != creators.end() && it->second)
    {
        return it->second();
    }

    return nullptr;
}

void AssetSaverFactory::registerSavers()
{
    creators["Cad"]            = []() { return std::make_unique<CadAssetSaver>(); };
    creators["Sdf"]            = []() { return std::make_unique<SdfAssetSaver>(); };
    creators["Texture"]        = []() { return std::make_unique<TextureAssetSaver>(); };
    creators["Triangulation"]  = []() { return std::make_unique<TriangulationAssetSaver>(); };
    creators["VectorGraphics"] = []() { return std::make_unique<VectorGraphicsAssetSaver>(); };
    creators["Voxel"]          = []() { return std::make_unique<VoxelAssetSaver>(); };
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("AssetSaverFactory/CreateSavers")
{
    AssetSaverFactory factory;
    CHECK(factory.create("Cad") != nullptr);
    CHECK(factory.create("Sdf") != nullptr);
    CHECK(factory.create("Texture") != nullptr);
    CHECK(factory.create("Triangulation") != nullptr);
    CHECK(factory.create("VectorGraphics") != nullptr);
    CHECK(factory.create("Voxel") != nullptr);
    CHECK(factory.create("Unknown") == nullptr);
}
#endif
