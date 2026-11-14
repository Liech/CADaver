#include "Rendering/Triangulation/IndexedFaceColorModule.h"

#include "Database/Asset/TriangulationAsset.h"
#include "Database/Component/FaceValuesComponent.h"
#include "Polyglot/Lua/LuaEngine.h"
#include "Polyglot/Node.h"
#include "Triangulation/Triangulation_IndexedFaceColor_frag.h"
#include "Vulcant/Interface/VulcantBuffer.h"
#include "Vulcant/Interface/VulcantDevice.h"
#include "Vulcant/Interface/VulcantResource.h"

#include <algorithm>
#include <exception>

IndexedFaceColorModule::IndexedFaceColorModule() {}

IndexedFaceColorModule::~IndexedFaceColorModule() {}

std::string IndexedFaceColorModule::getModuleKey() const
{
    return "FaceValues";
}

std::vector<uint32_t> IndexedFaceColorModule::getFragmentSpirv() const
{
    return std::vector<uint32_t>(Triangulation_IndexedFaceColor_frag_spirv, Triangulation_IndexedFaceColor_frag_spirv + Triangulation_IndexedFaceColor_frag_spirv_sizeInBytes / sizeof(uint32_t));
}

bool IndexedFaceColorModule::isCompatible(const std::string& componentType) const
{
    return componentType == "FaceValues";
}

std::vector<glm::vec4> IndexedFaceColorModule::evaluatePalette(const FaceValuesComponent& faceValues) const
{
    const auto&            entries = faceValues.getValues();
    std::vector<glm::vec4> palette;
    palette.reserve(std::max<size_t>(entries.size(), 1));

    if (entries.empty())
    {
        palette.push_back(glm::vec4(0.8f, 0.8f, 0.8f, 1.0f));
        return palette;
    }

    Polyglot::Lua::LuaEngine luaEngine;
    try
    {
        luaEngine.executeString(faceValues.getLuaScript());
        luaEngine.setVar("meta", Polyglot::Node(faceValues.getMetadata()));
    }
    catch (const std::exception&)
    {
        for (size_t i = 0; i < entries.size(); i++)
        {
            palette.push_back(glm::vec4(0.8f, 0.2f, 0.2f, 1.0f));
        }
        return palette;
    }

    for (const auto& entry : entries)
    {
        glm::vec4 color(0.8f, 0.8f, 0.8f, 1.0f);
        try
        {
            luaEngine.setVar("entry", Polyglot::Node(entry));
            luaEngine.executeString("local __c = getColor(entry, meta)\n__cadaverR = __c[1]\n__cadaverG = __c[2]\n__cadaverB = __c[3]\n");
            color.r = std::clamp(static_cast<float>(luaEngine.getVar("__cadaverR").asDouble()), 0.0f, 1.0f);
            color.g = std::clamp(static_cast<float>(luaEngine.getVar("__cadaverG").asDouble()), 0.0f, 1.0f);
            color.b = std::clamp(static_cast<float>(luaEngine.getVar("__cadaverB").asDouble()), 0.0f, 1.0f);
        }
        catch (const std::exception&)
        {
            color = glm::vec4(0.8f, 0.2f, 0.2f, 1.0f);
        }
        palette.push_back(color);
    }

    return palette;
}

void IndexedFaceColorModule::update(Vulcant::VulcantDevice& device, const TriangulationAsset& asset, const AssetComponent* component)
{
    const auto* faceValues = dynamic_cast<const FaceValuesComponent*>(component);
    if (!faceValues)
    {
        return;
    }

    if (faceValueIndicesBuffer && paletteColorsBuffer && lastComponentGuid == faceValues->getGuid() && lastComponentRevision == faceValues->getRevision() && lastAssetRevision == asset.getRevision())
    {
        return;
    }

    std::vector<glm::vec4> palette    = evaluatePalette(*faceValues);
    uint32_t               maxPalette = static_cast<uint32_t>(palette.size() - 1);

    size_t                triangleCount = asset.getTriangulation().indices.size() / 3;
    size_t                requiredFaces = std::max<size_t>(triangleCount, 1);
    const auto&           rawIndices    = faceValues->getFaceValueIndices();
    std::vector<uint32_t> safeIndices(requiredFaces, 0);
    size_t                copyCount = std::min(rawIndices.size(), requiredFaces);
    for (size_t i = 0; i < copyCount; i++)
    {
        safeIndices[i] = std::min(rawIndices[i], maxPalette);
    }

    if (!paletteColorsBuffer || palette.size() > paletteCapacity)
    {
        paletteCapacity     = palette.size();
        paletteColorsBuffer = device.createBuffer(paletteCapacity, sizeof(glm::vec4), false);
    }
    paletteColorsBuffer->uploadToGPU(palette.data(), palette.size(), 0);

    if (!faceValueIndicesBuffer || safeIndices.size() > faceIndexCapacity)
    {
        faceIndexCapacity      = safeIndices.size();
        faceValueIndicesBuffer = device.createBuffer(faceIndexCapacity, sizeof(uint32_t), false);
    }
    faceValueIndicesBuffer->uploadToGPU(safeIndices.data(), safeIndices.size(), 0);

    lastComponentGuid     = faceValues->getGuid();
    lastComponentRevision = faceValues->getRevision();
    lastAssetRevision     = asset.getRevision();
}

std::vector<std::shared_ptr<Vulcant::VulcantResource>> IndexedFaceColorModule::getSet1Resources() const
{
    if (!faceValueIndicesBuffer || !paletteColorsBuffer)
    {
        return {};
    }

    return { faceValueIndicesBuffer->asResource(), paletteColorsBuffer->asResource() };
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("IndexedFaceColorModule/EvaluatesDefaultLuaPalette")
{
    FaceValuesComponent faceValues;
    faceValues.setValues({
      { { "cluster", 0 }, { "faceCount", 2 } },
      { { "cluster", 1 }, { "faceCount", 1 } }
    });
    faceValues.setMetadata({
      { "componentCount", 2 },
      {      "faceCount", 3 }
    });

    IndexedFaceColorModule module;
    std::vector<glm::vec4> palette = module.evaluatePalette(faceValues);
    REQUIRE(palette.size() == 2);
    CHECK(palette[0].r > 0.8f);
    CHECK(palette[0].g < 0.3f);
    CHECK(palette[1].g > 0.7f);
}
#endif
