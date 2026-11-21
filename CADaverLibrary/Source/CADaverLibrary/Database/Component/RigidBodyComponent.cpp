#include "Database/Component/RigidBodyComponent.h"

RigidBodyComponent::RigidBodyComponent() {}

RigidBodyComponent::RigidBodyComponent(bool isStaticInput)
  : staticBody(isStaticInput)
{
}

RigidBodyComponent::~RigidBodyComponent() {}

std::string RigidBodyComponent::getType() const
{
    return "RigidBody";
}

std::string RigidBodyComponent::getDescription() const
{
    return "Component: RigidBody\n"
           "Purpose: Attaches rigid body physics properties to a triangulation, CAD, or voxel SVO asset for physics simulation.\n"
           "Compatibility: Compatible with Triangulation, Cad, and Voxel assets.\n\n"
           "Data Members:\n"
           "  - isStatic: Whether the rigid body is static (immovable) or dynamic (affected by gravity and collisions)";
}

size_t RigidBodyComponent::getSizeInBytes() const
{
    return sizeof(RigidBodyComponent);
}

bool RigidBodyComponent::isCompatible(const std::string& assetType) const
{
    return assetType == "Triangulation" || assetType == "Cad" || assetType == "Voxel";
}

nlohmann::json RigidBodyComponent::toDataJson() const
{
    return {
        {  "Version",          1 },
        { "isStatic", staticBody }
    };
}

void RigidBodyComponent::fromDataJson(const nlohmann::json& jsonObject)
{
    jsonObject.at("Version").get<int>();
    staticBody = jsonObject.at("isStatic").get<bool>();
    markModified();
}

bool RigidBodyComponent::isStatic() const
{
    return staticBody;
}

void RigidBodyComponent::setStatic(bool isStaticInput)
{
    staticBody = isStaticInput;
    markModified();
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("RigidBodyComponent/BasicPropertiesAndSerialization")
{
    RigidBodyComponent comp(true);
    CHECK(comp.getType() == "RigidBody");
    CHECK(comp.isStatic() == true);
    CHECK(comp.isCompatible("Triangulation") == true);
    CHECK(comp.isCompatible("Cad") == true);
    CHECK(comp.isCompatible("Voxel") == true);
    CHECK(comp.isCompatible("Progress") == false);

    comp.setStatic(false);
    CHECK(comp.isStatic() == false);

    nlohmann::json data = comp.toDataJson();
    CHECK(data.at("Version").get<int>() == 1);
    CHECK(data.at("isStatic").get<bool>() == false);

    RigidBodyComponent restored;
    restored.fromDataJson(data);
    CHECK(restored.isStatic() == false);
}
#endif
