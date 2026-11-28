#include "UI/ContextMenu/AssetContextMenuFactory.h"

#include "UI/ContextMenu/CadAssetContextMenu.h"
#include "UI/ContextMenu/ProgressAssetContextMenu.h"
#include "UI/ContextMenu/SdfAssetContextMenu.h"
#include "UI/ContextMenu/TextureAssetContextMenu.h"
#include "UI/ContextMenu/TriangulationAssetContextMenu.h"
#include "UI/ContextMenu/VectorGraphicsAssetContextMenu.h"
#include "UI/ContextMenu/VoxelAssetContextMenu.h"

AssetContextMenuFactory::AssetContextMenuFactory()
{
    registerMenus();
}

AssetContextMenuFactory::~AssetContextMenuFactory() {}

std::unique_ptr<AssetContextMenu> AssetContextMenuFactory::create(const std::string& assetType) const
{
    auto it = creators.find(assetType);
    if (it != creators.end() && it->second)
    {
        return it->second();
    }

    return nullptr;
}

void AssetContextMenuFactory::registerMenu(const std::string& assetType, std::function<std::unique_ptr<AssetContextMenu>()> creator)
{
    creators[assetType] = std::move(creator);
}

void AssetContextMenuFactory::registerMenus()
{
    registerMenu("Cad", []() { return std::make_unique<CadAssetContextMenu>(); });
    registerMenu("Progress", []() { return std::make_unique<ProgressAssetContextMenu>(); });
    registerMenu("Sdf", []() { return std::make_unique<SdfAssetContextMenu>(); });
    registerMenu("Texture", []() { return std::make_unique<TextureAssetContextMenu>(); });
    registerMenu("Triangulation", []() { return std::make_unique<TriangulationAssetContextMenu>(); });
    registerMenu("VectorGraphics", []() { return std::make_unique<VectorGraphicsAssetContextMenu>(); });
    registerMenu("Voxel", []() { return std::make_unique<VoxelAssetContextMenu>(); });
}
