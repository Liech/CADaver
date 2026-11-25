#include "IO/AssetLoader/VectorGraphicsAssetLoader.h"

#include "CADacomb/Util/ProgressBar.h"
#include "CADacomb/VectorGraphics/Svg/SvgReader.h"
#include "Database/Asset/VectorGraphicsAsset.h"
#include "Utility/IOValueGrabber.h"

#include <memory>
#include <string>
#include <vector>

VectorGraphicsAssetLoader::VectorGraphicsAssetLoader() {}

VectorGraphicsAssetLoader::~VectorGraphicsAssetLoader() {}

std::shared_ptr<Asset> VectorGraphicsAssetLoader::load(const std::string& name, const Polyglot::Node& source) const
{
    CADacomb::ProgressBar bar(3, "Loading SVG vector graphics asset");

    std::vector<uint8_t> bytes = IOValueGrabber::grabBytes(source);
    bar.addProgress(1);

    if (bytes.empty() || bar.isCanceled())
    {
        return nullptr;
    }

    std::string content(bytes.begin(), bytes.end());
    auto        document = std::make_shared<CADacomb::VectorGraphics::VectorGraphics>(CADacomb::VectorGraphics::SvgReader::loadFromString(content));
    bar.addProgress(1);

    if ((document->isEmpty() && !document->hasViewBox()) || bar.isCanceled())
    {
        return nullptr;
    }

    auto asset = std::make_shared<VectorGraphicsAsset>();
    asset->setName(name);
    asset->setVectorGraphics(document);

    bar.addProgress(1);
    return asset;
}
