#include "Database/Asset.h"

Asset::Asset()
  : guid(CADacomb::GUID::random())
{
}

Asset::~Asset() {}

const CADacomb::GUID& Asset::getGuid() const
{
    return guid;
}

const std::string& Asset::getName() const
{
    return name;
}

void Asset::setName(const std::string& newName)
{
    name = newName;
    markModified();
}

size_t Asset::getRevision() const
{
    return revision;
}

void Asset::markModified()
{
    revision++;
}

CADacomb::GUID Asset::addComponent(std::shared_ptr<AssetComponent> component)
{
    if (!component || !component->isCompatible(getType()))
    {
        return CADacomb::GUID();
    }

    CADacomb::GUID componentGuid = component->getGuid();
    components[componentGuid]    = std::move(component);
    markModified();
    return componentGuid;
}

bool Asset::removeComponent(const CADacomb::GUID& componentGuid)
{
    auto it = components.find(componentGuid);
    if (it != components.end())
    {
        components.erase(it);
        markModified();
        return true;
    }
    return false;
}

std::shared_ptr<AssetComponent> Asset::getComponent(const CADacomb::GUID& componentGuid) const
{
    auto it = components.find(componentGuid);
    if (it != components.end())
    {
        return it->second;
    }
    return nullptr;
}

std::vector<std::shared_ptr<AssetComponent>> Asset::getAttachments() const
{
    std::vector<std::shared_ptr<AssetComponent>> list;
    list.reserve(components.size());
    for (const auto& pair : components)
    {
        list.push_back(pair.second);
    }
    return list;
}

#ifdef ISTESTPROJECT
#include "Database/Component/TextNoteComponent.h"

#include <catch2/catch_test_macros.hpp>

class MockAsset : public Asset
{
  public:
    MockAsset() {}
    virtual ~MockAsset() {}

    std::string getType() const override
    {
        return "Mock";
    }
    size_t getSizeInBytes() const override
    {
        return sizeof(MockAsset);
    }
};

TEST_CASE("Asset/ComponentLifecycle")
{
    MockAsset asset;
    CHECK(asset.getAttachments().empty());

    auto note = asset.addComponent<TextNoteComponent>();
    REQUIRE(note != nullptr);
    note->setText("Test Note");
    CHECK(asset.getAttachments().size() == 1);
    CHECK(asset.getComponent(note->getGuid()) == note);

    auto notes = asset.getAttachments<TextNoteComponent>();
    CHECK(notes.size() == 1);
    CHECK(notes[0]->getText() == "Test Note");

    bool removed = asset.removeComponent(note->getGuid());
    CHECK(removed == true);
    CHECK(asset.getAttachments().empty());
    CHECK(asset.getComponent(note->getGuid()) == nullptr);
}
#endif
