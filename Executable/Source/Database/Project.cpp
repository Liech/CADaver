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

std::shared_ptr<Asset> Project::getAsset(const CADacomb::GUID& guid) const
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

const std::vector<std::shared_ptr<Asset>>& Project::getAssets() const
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

std::shared_ptr<Asset> Project::getSelectedAsset() const
{
    return getAsset(selectedGuid);
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

class MockAsset : public Asset
{
  public:
    MockAsset(const std::string& name)
      : Asset(name, "Triangulation")
    {
    }

    virtual ~MockAsset() {}

    size_t getSizeInBytes() const override
    {
        return 64;
    }
};

TEST_CASE("Project/AddAndGetAssetByGuid")
{
    Project project;
    CHECK(project.getAssetCount() == 0);
    CHECK(project.getSelectedAsset() == nullptr);

    auto           asset = std::make_shared<MockAsset>("Cube");
    CADacomb::GUID guid  = asset->getGuid();
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
    auto    asset1 = std::make_shared<MockAsset>("A");
    auto    asset2 = std::make_shared<MockAsset>("B");

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
