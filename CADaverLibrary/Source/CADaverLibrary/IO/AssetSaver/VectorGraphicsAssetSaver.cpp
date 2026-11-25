#include "IO/AssetSaver/VectorGraphicsAssetSaver.h"

#include "CADacomb/Util/ProgressBar.h"
#include "CADacomb/VectorGraphics/Svg/SvgWriter.h"
#include "Database/Asset/VectorGraphicsAsset.h"
#include "Utility/IOValueGrabber.h"

#include <string>
#include <vector>

VectorGraphicsAssetSaver::VectorGraphicsAssetSaver() {}

VectorGraphicsAssetSaver::~VectorGraphicsAssetSaver() {}

bool VectorGraphicsAssetSaver::save(const Asset& asset, const Polyglot::Node& destination) const
{
    CADacomb::ProgressBar bar(2, "Saving SVG vector graphics asset");
    if (bar.isCanceled())
    {
        return false;
    }

    const auto&          vectorAsset = static_cast<const VectorGraphicsAsset&>(asset);
    std::string          svgContent  = CADacomb::VectorGraphics::SvgWriter::saveToString(vectorAsset.getVectorGraphics());
    std::vector<uint8_t> bytes(svgContent.begin(), svgContent.end());
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
#include "CADacomb/VectorGraphics/Svg/SvgReader.h"
#include "IO/AssetLoader/VectorGraphicsAssetLoader.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("VectorGraphicsAssetSaver/RoundTripSvg")
{
    auto doc = std::make_shared<CADacomb::VectorGraphics::VectorGraphics>(CADacomb::VectorGraphics::SvgReader::loadFromString(
      R"(<svg viewBox="0 0 100 50"><g id="grp"><rect x="10" y="10" width="30" height="20" fill="#112233"/><text x="5" y="40">Test</text></g></svg>)"));

    VectorGraphicsAsset asset;
    asset.setVectorGraphics(doc);

    std::vector<uint8_t> savedBytes;
    Polyglot::Node       destCallback(std::function<Polyglot::Node(const Polyglot::Node&)>(
      [&savedBytes](const Polyglot::Node& data) -> Polyglot::Node
      {
          savedBytes = data.asBinary();
          return Polyglot::Node(true);
      }));

    VectorGraphicsAssetSaver saver;
    REQUIRE(saver.save(asset, destCallback));
    REQUIRE_FALSE(savedBytes.empty());

    VectorGraphicsAssetLoader loader;
    auto                      loaded = std::dynamic_pointer_cast<VectorGraphicsAsset>(loader.load("TestSvg", Polyglot::Node(savedBytes)));
    REQUIRE(loaded != nullptr);
    CHECK(loaded->getVectorGraphics().hasViewBox());
    CHECK(loaded->getVectorGraphics().getElements().size() == 1);
}
#endif
