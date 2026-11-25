#include "Database/Component/UVComponent.h"

UVComponent::UVComponent() {}

UVComponent::~UVComponent() {}

std::string UVComponent::getType() const
{
    return "UV";
}

std::string UVComponent::getDescription() const
{
    return "Component: UV\nAttaches per-face-corner UV coordinates to a Triangulation asset.";
}

size_t UVComponent::getSizeInBytes() const
{
    return sizeof(UVComponent) + uvs.getCornerCount() * sizeof(glm::dvec2);
}

bool UVComponent::isCompatible(const std::string& assetType) const
{
    return assetType == "Triangulation";
}

nlohmann::json UVComponent::toDataJson() const
{
    nlohmann::json uvArray = nlohmann::json::array();
    for (const auto& uv : uvs.getAllUVs())
    {
        uvArray.push_back(uv.x);
        uvArray.push_back(uv.y);
    }

    return nlohmann::json{
        { "Version",       1 },
        {     "uvs", uvArray }
    };
}

void UVComponent::fromDataJson(const nlohmann::json& jsonObject)
{
    int version = jsonObject.at("Version").get<int>();
    (void)version;

    const auto& uvArray     = jsonObject.at("uvs");
    size_t      cornerCount = uvArray.size() / 2;
    uvs.resize(cornerCount);
    for (size_t i = 0; i < cornerCount; i++)
    {
        uvs.setUV(i, glm::dvec2(uvArray[i * 2].get<double>(), uvArray[i * 2 + 1].get<double>()));
    }

    markModified();
}

const CADacomb::Texture::TriangulationUVs& UVComponent::getUVs() const
{
    return uvs;
}

CADacomb::Texture::TriangulationUVs& UVComponent::getUVs()
{
    return uvs;
}

void UVComponent::setUVs(const CADacomb::Texture::TriangulationUVs& newUVs)
{
    uvs = newUVs;
    markModified();
}

#ifdef ISTESTPROJECT
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("UVComponent/JsonRoundTrip")
{
    UVComponent component;
    CHECK(component.getType() == "UV");
    CHECK(component.isCompatible("Triangulation"));
    CHECK_FALSE(component.isCompatible("Voxel"));

    CADacomb::Texture::TriangulationUVs uvMap(3);
    uvMap.setUV(0, glm::dvec2(0.0, 1.0));
    uvMap.setUV(1, glm::dvec2(1.0, 1.0));
    uvMap.setUV(2, glm::dvec2(0.5, 0.0));
    component.setUVs(uvMap);

    nlohmann::json serialized = component.toDataJson();
    UVComponent    restored;
    restored.fromDataJson(serialized);

    REQUIRE(restored.getUVs().getCornerCount() == 3);
    CHECK(restored.getUVs().getUV(0).x == Catch::Approx(0.0));
    CHECK(restored.getUVs().getUV(0).y == Catch::Approx(1.0));
    CHECK(restored.getUVs().getUV(2).x == Catch::Approx(0.5));
    CHECK(restored.getUVs().getUV(2).y == Catch::Approx(0.0));
}
#endif
