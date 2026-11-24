#include "IO/AssetLoader/TextureAssetLoader.h"

#include "CADacomb/Texture/Texture.h"
#include "CADacomb/Util/ProgressBar.h"
#include "Database/Asset/TextureAsset.h"
#include "Utility/IOValueGrabber.h"

#include <memory>
#include <vector>

TextureAssetLoader::TextureAssetLoader() {}

TextureAssetLoader::~TextureAssetLoader() {}

std::shared_ptr<Asset> TextureAssetLoader::load(const std::string& name, const Polyglot::Node& source) const
{
    CADacomb::ProgressBar bar(3, "Loading PNG texture asset");

    std::vector<uint8_t> bytes = IOValueGrabber::grabBytes(source);
    bar.addProgress(1);

    if (bytes.empty() || bar.isCanceled())
    {
        return nullptr;
    }

    auto texture = std::make_shared<CADacomb::Texture::Texture>(CADacomb::Texture::Texture::loadFromPngMemory(bytes));
    bar.addProgress(1);

    if (texture->isEmpty() || bar.isCanceled())
    {
        return nullptr;
    }

    auto asset = std::make_shared<TextureAsset>();
    asset->setName(name);
    asset->setTexture(texture);

    bar.addProgress(1);
    return asset;
}
