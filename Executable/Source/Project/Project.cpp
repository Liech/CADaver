#include "Project.h"

Project::Project() {}

Project::~Project() {}

void Project::addAsset(std::shared_ptr<Asset> asset)
{
    if (asset)
    {
        assets.push_back(asset);
        if (selectedIndex < 0)
        {
            selectedIndex = 0;
        }
    }
}

void Project::removeAsset(size_t index)
{
    if (index < assets.size())
    {
        assets.erase(assets.begin() + index);
        if (assets.empty())
        {
            selectedIndex = -1;
        }
        else if (selectedIndex >= static_cast<int>(assets.size()))
        {
            selectedIndex = static_cast<int>(assets.size()) - 1;
        }
    }
}

void Project::clear()
{
    assets.clear();
    selectedIndex = -1;
}

size_t Project::getAssetCount() const
{
    return assets.size();
}

std::shared_ptr<Asset> Project::getAsset(size_t index) const
{
    if (index < assets.size())
    {
        return assets[index];
    }
    return nullptr;
}

const std::vector<std::shared_ptr<Asset>>& Project::getAssets() const
{
    return assets;
}

int Project::getSelectedIndex() const
{
    return selectedIndex;
}

void Project::setSelectedIndex(int index)
{
    selectedIndex = index;
}

std::shared_ptr<Asset> Project::getSelectedAsset() const
{
    if (selectedIndex >= 0 && selectedIndex < static_cast<int>(assets.size()))
    {
        return assets[selectedIndex];
    }
    return nullptr;
}
