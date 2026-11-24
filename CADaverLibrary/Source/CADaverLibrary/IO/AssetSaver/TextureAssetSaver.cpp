#include "IO/AssetSaver/TextureAssetSaver.h"

#include "CADacomb/Util/ProgressBar.h"
#include "Database/Asset/TextureAsset.h"
#include "Utility/IOValueGrabber.h"

#include <vector>

TextureAssetSaver::TextureAssetSaver() {}

TextureAssetSaver::~TextureAssetSaver() {}

bool TextureAssetSaver::save(const Asset& asset, const Polyglot::Node& destination) const
{
    CADacomb::ProgressBar bar(2, "Saving PNG texture asset");
    if (bar.isCanceled())
    {
        return false;
    }

    const auto&          textureAsset = static_cast<const TextureAsset&>(asset);
    std::vector<uint8_t> bytes        = textureAsset.getTexture().encodeToPngMemory();
    bar.addProgress(1);

    if (bar.isCanceled())
    {
        return false;
    }

    if (!IOValueGrabber::deliverBytes(destination, bytes))
    {
        return false;
    }

    bar.addProgress(1);
    return true;
}

#ifdef ISTESTPROJECT
#include "IO/AssetLoader/TextureAssetLoader.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("TextureAssetSaver/RoundTripPng")
{
    auto tex = std::make_shared<CADacomb::Texture::Texture>(2, 2, glm::u8vec4(10, 20, 30, 255));
    tex->setPixel(1, 1, glm::u8vec4(250, 125, 0, 255));

    TextureAsset asset;
    asset.setTexture(tex);

    std::vector<uint8_t> savedBytes;
    Polyglot::Node       destCallback(std::function<Polyglot::Node(const Polyglot::Node&)>(
      [&savedBytes](const Polyglot::Node& data) -> Polyglot::Node
      {
          savedBytes = data.asBinary();
          return Polyglot::Node(true);
      }));

    TextureAssetSaver saver;
    REQUIRE(saver.save(asset, destCallback));
    REQUIRE_FALSE(savedBytes.empty());

    TextureAssetLoader loader;
    auto               loaded = std::dynamic_pointer_cast<TextureAsset>(loader.load("TestTex", Polyglot::Node(savedBytes)));
    REQUIRE(loaded != nullptr);
    CHECK(loaded->getTexture().getWidth() == 2);
    CHECK(loaded->getTexture().getHeight() == 2);
    CHECK(loaded->getTexture().getPixel(1, 1) == glm::u8vec4(250, 125, 0, 255));
}
#endif
