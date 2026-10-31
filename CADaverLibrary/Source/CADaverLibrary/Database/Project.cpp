#include "Database/Project.h"

#include <algorithm>

Project::Project() {}

Project::~Project() {}

void Project::addAsset(std::shared_ptr<Asset> asset)
{
    if (asset)
    {
        assets.push_back(asset);
        if (selectedGuid.toString().empty())
        {
            selectedGuid = asset->getGuid();
        }
    }
}

void Project::removeAsset(const CADacomb::GUID& guid)
{
    auto it = std::find_if(assets.begin(), assets.end(), [&guid](const std::shared_ptr<Asset>& a) { return a && a->getGuid() == guid; });
    if (it != assets.end())
    {
        bool wasSelected = (selectedGuid == guid);
        assets.erase(it);

        if (assets.empty())
        {
            selectedGuid = CADacomb::GUID();
        }
        else if (wasSelected)
        {
            selectedGuid = assets.front()->getGuid();
        }
    }
}

void Project::clear()
{
    assets.clear();
    selectedGuid = CADacomb::GUID();
}

size_t Project::getAssetCount() const
{
    return assets.size();
}

bool Project::hasAsset(const CADacomb::GUID& guid) const
{
    return getAsset(guid) != nullptr;
}

std::shared_ptr<const Asset> Project::getAsset(const CADacomb::GUID& guid) const
{
    for (const auto& asset : assets)
    {
        if (asset && asset->getGuid() == guid)
        {
            return asset;
        }
    }
    return nullptr;
}

std::shared_ptr<Asset> Project::getAsset(const CADacomb::GUID& guid)
{
    for (const auto& asset : assets)
    {
        if (asset && asset->getGuid() == guid)
        {
            return asset;
        }
    }
    return nullptr;
}

std::vector<std::shared_ptr<const Asset>> Project::getAssets() const
{
    return std::vector<std::shared_ptr<const Asset>>(assets.begin(), assets.end());
}

const std::vector<std::shared_ptr<Asset>>& Project::getAssets()
{
    return assets;
}

CADacomb::GUID Project::getSelectedAssetGuid() const
{
    return selectedGuid;
}

void Project::setSelectedAssetGuid(const CADacomb::GUID& guid)
{
    selectedGuid = guid;
}

std::shared_ptr<const Asset> Project::getSelectedAsset() const
{
    return getAsset(selectedGuid);
}

std::shared_ptr<Asset> Project::getSelectedAsset()
{
    return getAsset(selectedGuid);
}

#ifdef ISTESTPROJECT
#include "Database/Asset/TriangulationAsset.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Project/AddAndGetAssetByGuid")
{
    Project project;
    CHECK(project.getAssetCount() == 0);
    CHECK(project.getSelectedAsset() == nullptr);

    auto asset = std::make_shared<TriangulationAsset>();
    asset->setName("Cube");
    CADacomb::GUID guid = asset->getGuid();
    CHECK_FALSE(guid.toString().empty());

    project.addAsset(asset);
    CHECK(project.getAssetCount() == 1);
    CHECK(project.hasAsset(guid));
    CHECK(project.getAsset(guid) == asset);
    CHECK(project.getSelectedAssetGuid() == guid);
    CHECK(project.getSelectedAsset() == asset);
}

TEST_CASE("Project/RemoveAssetByGuid")
{
    Project project;
    auto    asset1 = std::make_shared<TriangulationAsset>();
    asset1->setName("A");
    auto asset2 = std::make_shared<TriangulationAsset>();
    asset2->setName("B");

    project.addAsset(asset1);
    project.addAsset(asset2);
    CHECK(project.getAssetCount() == 2);

    project.setSelectedAssetGuid(asset2->getGuid());
    CHECK(project.getSelectedAsset() == asset2);

    project.removeAsset(asset2->getGuid());
    CHECK(project.getAssetCount() == 1);
    CHECK_FALSE(project.hasAsset(asset2->getGuid()));
    CHECK(project.getSelectedAssetGuid() == asset1->getGuid());

    project.clear();
    CHECK(project.getAssetCount() == 0);
    CHECK(project.getSelectedAsset() == nullptr);
}
#endif
