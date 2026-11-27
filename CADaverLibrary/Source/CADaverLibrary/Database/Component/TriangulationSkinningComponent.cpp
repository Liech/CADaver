#include "Database/Component/TriangulationSkinningComponent.h"

TriangulationSkinningComponent::TriangulationSkinningComponent() {}

TriangulationSkinningComponent::~TriangulationSkinningComponent() {}

std::string TriangulationSkinningComponent::getType() const
{
    return "TriangulationSkinning";
}

std::string TriangulationSkinningComponent::getDescription() const
{
    return "Component: TriangulationSkinning\nAttaches per-vertex skeletal joint indices and weights to a Triangulation asset.";
}

size_t TriangulationSkinningComponent::getSizeInBytes() const
{
    return sizeof(TriangulationSkinningComponent) + skinning.getVertexCount() * sizeof(CADacomb::Animation::VertexSkinWeights);
}

bool TriangulationSkinningComponent::isCompatible(const std::string& assetType) const
{
    return assetType == "Triangulation";
}

nlohmann::json TriangulationSkinningComponent::toDataJson() const
{
    nlohmann::json indicesArray = nlohmann::json::array();
    nlohmann::json weightsArray = nlohmann::json::array();

    for (const auto& vw : skinning.getAllVertexWeights())
    {
        const auto& idx = vw.getJointIndices();
        const auto& w   = vw.getWeights();
        indicesArray.push_back(idx.x);
        indicesArray.push_back(idx.y);
        indicesArray.push_back(idx.z);
        indicesArray.push_back(idx.w);
        weightsArray.push_back(w.x);
        weightsArray.push_back(w.y);
        weightsArray.push_back(w.z);
        weightsArray.push_back(w.w);
    }

    return nlohmann::json{
        {      "Version",            1 },
        { "jointIndices", indicesArray },
        {      "weights", weightsArray }
    };
}

void TriangulationSkinningComponent::fromDataJson(const nlohmann::json& jsonObject)
{
    int version = jsonObject.at("Version").get<int>();
    (void)version;

    const auto& indicesArray = jsonObject.at("jointIndices");
    const auto& weightsArray = jsonObject.at("weights");
    size_t      vertexCount  = indicesArray.size() / 4;

    skinning.resize(vertexCount);
    for (size_t i = 0; i < vertexCount; i++)
    {
        size_t     base = i * 4;
        glm::uvec4 idx(indicesArray[base].get<uint32_t>(), indicesArray[base + 1].get<uint32_t>(), indicesArray[base + 2].get<uint32_t>(), indicesArray[base + 3].get<uint32_t>());
        glm::dvec4 w(weightsArray[base].get<double>(), weightsArray[base + 1].get<double>(), weightsArray[base + 2].get<double>(), weightsArray[base + 3].get<double>());
        skinning.setVertexWeights(i, CADacomb::Animation::VertexSkinWeights(idx, w));
    }

    markModified();
}

const CADacomb::Animation::TriangulationSkinning& TriangulationSkinningComponent::getSkinning() const
{
    return skinning;
}

CADacomb::Animation::TriangulationSkinning& TriangulationSkinningComponent::getSkinning()
{
    return skinning;
}

void TriangulationSkinningComponent::setSkinning(const CADacomb::Animation::TriangulationSkinning& newSkinning)
{
    skinning = newSkinning;
    markModified();
}

#ifdef ISTESTPROJECT
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("TriangulationSkinningComponent/JsonRoundTrip")
{
    TriangulationSkinningComponent component;
    CHECK(component.getType() == "TriangulationSkinning");
    CHECK(component.isCompatible("Triangulation"));
    CHECK_FALSE(component.isCompatible("Voxel"));

    CADacomb::Animation::TriangulationSkinning skin(2);
    skin.getVertexWeights(0).addInfluence(1, 0.75);
    skin.getVertexWeights(0).addInfluence(2, 0.25);
    skin.getVertexWeights(1).addInfluence(0, 1.0);
    skin.normalizeAll();
    component.setSkinning(skin);

    nlohmann::json                 serialized = component.toDataJson();
    TriangulationSkinningComponent restored;
    restored.fromDataJson(serialized);

    REQUIRE(restored.getSkinning().getVertexCount() == 2);
    CHECK(restored.getSkinning().getVertexWeights(0).getJointIndices().x == 1);
    CHECK(restored.getSkinning().getVertexWeights(0).getJointIndices().y == 2);
    CHECK(restored.getSkinning().getVertexWeights(0).getWeights().x == Catch::Approx(0.75));
    CHECK(restored.getSkinning().getVertexWeights(0).getWeights().y == Catch::Approx(0.25));
    CHECK(restored.getSkinning().getVertexWeights(1).getWeights().x == Catch::Approx(1.0));
}
#endif
