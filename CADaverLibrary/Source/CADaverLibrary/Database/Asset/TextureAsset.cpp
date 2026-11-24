#include "Database/Asset/TextureAsset.h"

TextureAsset::TextureAsset()
  : texture(std::make_shared<CADacomb::Texture::Texture>())
{
}

TextureAsset::~TextureAsset() {}

std::string TextureAsset::getType() const
{
    return "Texture";
}

const CADacomb::Texture::Texture& TextureAsset::getTexture() const
{
    return *texture;
}

CADacomb::Texture::Texture& TextureAsset::getTexture()
{
    return *texture;
}

void TextureAsset::setTexture(std::shared_ptr<CADacomb::Texture::Texture> newTexture)
{
    texture = std::move(newTexture);
    if (texture && texture->getHeight() > 0)
    {
        double aspect = static_cast<double>(texture->getWidth()) / static_cast<double>(texture->getHeight());
        worldSize     = glm::dvec2(aspect, 1.0);
    }
    markModified();
}

const glm::dvec2& TextureAsset::getWorldSize() const
{
    return worldSize;
}

void TextureAsset::setWorldSize(const glm::dvec2& size)
{
    worldSize = size;
    markModified();
}

size_t TextureAsset::getSizeInBytes() const
{
    size_t total = sizeof(TextureAsset);
    if (texture)
    {
        total += texture->getByteSize();
    }
    return total;
}

#ifdef ISTESTPROJECT
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("TextureAsset/PropertiesAndAspectRatio")
{
    TextureAsset asset;
    CHECK(asset.getType() == "Texture");

    auto tex = std::make_shared<CADacomb::Texture::Texture>(8, 4, glm::u8vec4(255, 0, 0, 255));
    asset.setTexture(tex);

    CHECK(asset.getTexture().getWidth() == 8);
    CHECK(asset.getTexture().getHeight() == 4);
    CHECK(asset.getWorldSize().x == Catch::Approx(2.0));
    CHECK(asset.getWorldSize().y == Catch::Approx(1.0));
    CHECK(asset.getSizeInBytes() >= 8 * 4 * 4);
}
#endif
