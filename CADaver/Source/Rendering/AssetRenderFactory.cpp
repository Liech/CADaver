#include "AssetRenderFactory.h"

#include "AssetRenderer.h"
#include "StepAssetRenderer.h"
#include "TriangulationAssetRenderer.h"
#include "VoxelAssetRenderer.h"

AssetRenderFactory::AssetRenderFactory()
{
    registerRenderers();
}

AssetRenderFactory::~AssetRenderFactory() {}

AssetRenderFactory& AssetRenderFactory::getInstance()
{
    static AssetRenderFactory instance;
    return instance;
}

std::shared_ptr<AssetRenderer> AssetRenderFactory::create(const std::string& type) const
{
    auto it = renderMap.find(type);
    if (it != renderMap.end() && it->second)
    {
        return it->second();
    }
    return nullptr;
}

void AssetRenderFactory::registerRenderer(const std::string& type, std::function<std::shared_ptr<AssetRenderer>()> creator)
{
    renderMap[type] = std::move(creator);
}

void AssetRenderFactory::registerRenderers()
{
    registerRenderer("Triangulation", []() { return std::make_shared<TriangulationAssetRenderer>(); });
    registerRenderer("Cad", []() { return std::make_shared<StepAssetRenderer>(); });
    registerRenderer("Voxel", []() { return std::make_shared<VoxelAssetRenderer>(); });
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("AssetRenderFactory/SingletonInstance")
{
    auto& instance1 = AssetRenderFactory::getInstance();
    auto& instance2 = AssetRenderFactory::getInstance();
    CHECK(&instance1 == &instance2);
}

TEST_CASE("AssetRenderFactory/CreateUnknownType")
{
    AssetRenderFactory factory;
    auto               renderer = factory.create("UnknownType");
    CHECK(renderer == nullptr);
}
#endif
