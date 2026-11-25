#include "Database/Component/TextureComponent.h"

#include "CADacomb/Util/Binary/base64.h"

#include <cstdint>
#include <vector>

TextureComponent::TextureComponent()
  : texture(std::make_shared<CADacomb::Texture::Texture>())
{
}

TextureComponent::~TextureComponent() {}

std::string TextureComponent::getType() const
{
    return "Texture";
}

std::string TextureComponent::getDescription() const
{
    return "Component: Texture\nAttaches a 2D diffuse texture to a Triangulation asset.";
}

size_t TextureComponent::getSizeInBytes() const
{
    size_t total = sizeof(TextureComponent);
    if (texture)
    {
        total += texture->getByteSize();
    }
    return total;
}

bool TextureComponent::isCompatible(const std::string& assetType) const
{
    return assetType == "Triangulation";
}

nlohmann::json TextureComponent::toDataJson() const
{
    std::string pngBase64;
    if (texture && !texture->isEmpty())
    {
        std::vector<uint8_t> pngBytes = texture->encodeToPngMemory();
        pngBase64                     = CADacomb::base64::base64_encode(pngBytes.data(), pngBytes.size());
    }

    return nlohmann::json{
        {   "Version",         1 },
        { "pngBase64", pngBase64 }
    };
}

void TextureComponent::fromDataJson(const nlohmann::json& jsonObject)
{
    int version = jsonObject.at("Version").get<int>();
    (void)version;

    std::string pngBase64 = jsonObject.at("pngBase64").get<std::string>();
    if (!pngBase64.empty())
    {
        std::vector<uint8_t> pngBytes = CADacomb::base64::base64_decode(pngBase64);
        texture                       = std::make_shared<CADacomb::Texture::Texture>(CADacomb::Texture::Texture::loadFromPngMemory(pngBytes));
    }
    else
    {
        texture = std::make_shared<CADacomb::Texture::Texture>();
    }

    markModified();
}

const CADacomb::Texture::Texture& TextureComponent::getTexture() const
{
    return *texture;
}

CADacomb::Texture::Texture& TextureComponent::getTexture()
{
    return *texture;
}

void TextureComponent::setTexture(std::shared_ptr<CADacomb::Texture::Texture> newTexture)
{
    texture = std::move(newTexture);
    markModified();
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("TextureComponent/JsonRoundTrip")
{
    TextureComponent component;
    CHECK(component.getType() == "Texture");
    CHECK(component.isCompatible("Triangulation"));
    CHECK_FALSE(component.isCompatible("Voxel"));

    auto tex = std::make_shared<CADacomb::Texture::Texture>(2, 2, glm::u8vec4(10, 20, 30, 255));
    tex->setPixel(1, 0, glm::u8vec4(200, 150, 100, 255));
    component.setTexture(tex);

    nlohmann::json   serialized = component.toDataJson();
    TextureComponent restored;
    restored.fromDataJson(serialized);

    CHECK(restored.getTexture().getWidth() == 2);
    CHECK(restored.getTexture().getHeight() == 2);
    CHECK(restored.getTexture().getPixel(1, 0) == glm::u8vec4(200, 150, 100, 255));
}
#endif
