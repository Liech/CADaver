#include "UI/ContextMenu/AssetContextMenu.h"

AssetContextMenu::AssetContextMenu() {}

AssetContextMenu::~AssetContextMenu() {}

const CADacomb::GUID& AssetContextMenu::getAssetGuid() const
{
    return assetGuid;
}

void AssetContextMenu::setAssetGuid(const CADacomb::GUID& guid)
{
    assetGuid = guid;
}
