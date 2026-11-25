#include "Rendering/Triangulation/TexturedSurfaceModule.h"

#include "CADacombRender/Texture/TextureImageConverter.h"
#include "Database/Asset/TriangulationAsset.h"
#include "Database/Component/TextureComponent.h"
#include "Database/Component/UVComponent.h"
#include "Triangulation/Triangulation_Textured_frag.h"
#include "Vulcant/Interface/VulcantBuffer.h"
#include "Vulcant/Interface/VulcantDevice.h"
#include "Vulcant/Interface/VulcantImage.h"
#include "Vulcant/Interface/VulcantImageUsage.h"
#include "Vulcant/Interface/VulcantResource.h"

#include <algorithm>
#include <glm/glm.hpp>
#include <vector>

TexturedSurfaceModule::TexturedSurfaceModule() {}

TexturedSurfaceModule::~TexturedSurfaceModule() {}

std::string TexturedSurfaceModule::getModuleKey() const
{
    return "Texture";
}

std::vector<uint32_t> TexturedSurfaceModule::getFragmentSpirv() const
{
    return std::vector<uint32_t>(Triangulation_Textured_frag_spirv, Triangulation_Textured_frag_spirv + Triangulation_Textured_frag_spirv_sizeInBytes / sizeof(uint32_t));
}

bool TexturedSurfaceModule::isCompatible(const std::string& componentType) const
{
    return componentType == "Texture";
}

void TexturedSurfaceModule::update(Vulcant::VulcantDevice& device, const TriangulationAsset& asset, const AssetComponent* component)
{
    const auto* textureComponent = dynamic_cast<const TextureComponent*>(component);
    auto        uvComponents     = asset.getAttachments<UVComponent>();
    if (!textureComponent || textureComponent->getTexture().isEmpty() || uvComponents.empty())
    {
        return;
    }

    const auto& uvComponent = uvComponents.front();
    if (uvBuffer && textureImage && lastComponentGuid == textureComponent->getGuid() && lastComponentRevision == textureComponent->getRevision() && lastUvComponentGuid == uvComponent->getGuid() &&
        lastUvComponentRevision == uvComponent->getRevision() && lastAssetRevision == asset.getRevision())
    {
        return;
    }

    size_t                 requiredCorners = std::max<size_t>(asset.getTriangulation().indices.size(), 3);
    const auto&            rawUVs          = uvComponent->getUVs().getAllUVs();
    std::vector<glm::vec2> gpuUVs(requiredCorners, glm::vec2(0.0f, 0.0f));
    size_t                 copyCount = std::min(rawUVs.size(), requiredCorners);
    for (size_t i = 0; i < copyCount; i++)
    {
        gpuUVs[i] = glm::vec2(static_cast<float>(rawUVs[i].x), static_cast<float>(rawUVs[i].y));
    }

    if (!uvBuffer || gpuUVs.size() > uvCapacity)
    {
        uvCapacity = gpuUVs.size();
        uvBuffer   = device.createBuffer(uvCapacity, sizeof(glm::vec2), false);
    }
    uvBuffer->uploadToGPU(gpuUVs.data(), gpuUVs.size(), 0);

    textureImage = CADacombRender::TextureImageConverter::toVulcantImage(device, textureComponent->getTexture(), Vulcant::VulcantImageUsage::SampleOnly);

    lastComponentGuid       = textureComponent->getGuid();
    lastComponentRevision   = textureComponent->getRevision();
    lastUvComponentGuid     = uvComponent->getGuid();
    lastUvComponentRevision = uvComponent->getRevision();
    lastAssetRevision       = asset.getRevision();
}

std::vector<std::shared_ptr<Vulcant::VulcantResource>> TexturedSurfaceModule::getSet1Resources() const
{
    if (!uvBuffer || !textureImage)
    {
        return {};
    }

    return { uvBuffer->asResource(), textureImage->asResource() };
}
