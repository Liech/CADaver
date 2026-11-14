#include "Picking/AssetRayPicker/AssetRayPickerFactory.h"

#include "Picking/AssetRayPicker/CadAssetRayPicker.h"
#include "Picking/AssetRayPicker/TriangulationAssetRayPicker.h"
#include "Picking/AssetRayPicker/VoxelAssetRayPicker.h"

AssetRayPickerFactory::AssetRayPickerFactory()
{
    registerPickers();
}

AssetRayPickerFactory::~AssetRayPickerFactory() {}

std::unique_ptr<AssetRayPicker> AssetRayPickerFactory::create(const std::string& assetType) const
{
    auto it = creators.find(assetType);
    if (it != creators.end() && it->second)
    {
        return it->second();
    }

    return nullptr;
}

void AssetRayPickerFactory::registerPicker(const std::string& assetType, std::function<std::unique_ptr<AssetRayPicker>()> creator)
{
    creators[assetType] = std::move(creator);
}

void AssetRayPickerFactory::registerPickers()
{
    registerPicker("Triangulation", []() { return std::make_unique<TriangulationAssetRayPicker>(); });
    registerPicker("Cad", []() { return std::make_unique<CadAssetRayPicker>(); });
    registerPicker("Voxel", []() { return std::make_unique<VoxelAssetRayPicker>(); });
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("AssetRayPickerFactory/CreatePickers")
{
    AssetRayPickerFactory factory;
    CHECK(factory.create("Triangulation") != nullptr);
    CHECK(factory.create("Cad") != nullptr);
    CHECK(factory.create("Voxel") != nullptr);
    CHECK(factory.create("Unknown") == nullptr);
}
#endif
