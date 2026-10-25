#include "AssetContextMenu.h"

AssetContextMenu::AssetContextMenu() {}

AssetContextMenu::~AssetContextMenu() {}

int AssetContextMenu::getAssetIndex() const
{
    return assetIndex;
}

void AssetContextMenu::setAssetIndex(int index)
{
    assetIndex = index;
}
