#include "Database/Component/ComponentFactory.h"

#include "Database/Component/FaceValuesComponent.h"
#include "Database/Component/RigidBodyComponent.h"
#include "Database/Component/TextNoteComponent.h"

ComponentFactory::ComponentFactory()
{
    registerComponents();
}

ComponentFactory::~ComponentFactory() {}

std::shared_ptr<AssetComponent> ComponentFactory::create(const std::string& type) const
{
    auto it = creators.find(type);
    if (it != creators.end() && it->second)
    {
        return it->second();
    }
    return nullptr;
}

std::vector<std::string> ComponentFactory::getRegisteredTypes() const
{
    std::vector<std::string> types;
    types.reserve(creators.size());
    for (const auto& pair : creators)
    {
        types.push_back(pair.first);
    }
    return types;
}

void ComponentFactory::registerComponent(const std::string& type, std::function<std::shared_ptr<AssetComponent>()> creator)
{
    creators[type] = std::move(creator);
}

void ComponentFactory::registerComponents()
{
    registerComponent("TextNote", []() { return std::make_shared<TextNoteComponent>(); });
    registerComponent("FaceValues", []() { return std::make_shared<FaceValuesComponent>(); });
    registerComponent("RigidBody", []() { return std::make_shared<RigidBodyComponent>(); });
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("ComponentFactory/CreateRegisteredComponents")
{
    ComponentFactory factory;
    CHECK(factory.create("Unknown") == nullptr);

    auto note = factory.create("TextNote");
    REQUIRE(note != nullptr);
    CHECK(note->getType() == "TextNote");

    auto faceValues = factory.create("FaceValues");
    REQUIRE(faceValues != nullptr);
    CHECK(faceValues->getType() == "FaceValues");

    auto rigidBody = factory.create("RigidBody");
    REQUIRE(rigidBody != nullptr);
    CHECK(rigidBody->getType() == "RigidBody");
}
#endif
