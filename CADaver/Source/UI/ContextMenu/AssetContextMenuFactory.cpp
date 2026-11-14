#include "UI/ContextMenu/AssetContextMenuFactory.h"

#include "UI/ContextMenu/CadAssetContextMenu.h"
#include "UI/ContextMenu/TriangulationAssetContextMenu.h"
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
    registerMenu("Triangulation", []() { return std::make_unique<TriangulationAssetContextMenu>(); });
    registerMenu("Voxel", []() { return std::make_unique<VoxelAssetContextMenu>(); });
}
