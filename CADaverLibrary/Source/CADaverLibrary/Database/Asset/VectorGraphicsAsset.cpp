#include "Database/Asset/VectorGraphicsAsset.h"

VectorGraphicsAsset::VectorGraphicsAsset()
  : vectorGraphics(std::make_shared<CADacomb::VectorGraphics::VectorGraphics>())
{
}

VectorGraphicsAsset::~VectorGraphicsAsset() {}

std::string VectorGraphicsAsset::getType() const
{
    return "VectorGraphics";
}

const CADacomb::VectorGraphics::VectorGraphics& VectorGraphicsAsset::getVectorGraphics() const
{
    return *vectorGraphics;
}

CADacomb::VectorGraphics::VectorGraphics& VectorGraphicsAsset::getVectorGraphics()
{
    return *vectorGraphics;
}

void VectorGraphicsAsset::setVectorGraphics(std::shared_ptr<CADacomb::VectorGraphics::VectorGraphics> document)
{
    vectorGraphics = std::move(document);
    if (vectorGraphics)
    {
        glm::dvec2 docSize(0.0, 0.0);
        if (vectorGraphics->hasViewBox())
        {
            docSize = vectorGraphics->getViewBoxSize();
        }
        else
        {
            CADacomb::VectorGraphics::VectorBounds bounds = vectorGraphics->computeBounds();
            if (bounds.isValid())
            {
                docSize = bounds.getSize();
            }
        }

        if (docSize.x > 1e-9 && docSize.y > 1e-9)
        {
            double aspect = docSize.x / docSize.y;
            worldSize     = glm::dvec2(aspect, 1.0);
        }
    }
    markModified();
}

const glm::dvec2& VectorGraphicsAsset::getWorldSize() const
{
    return worldSize;
}

void VectorGraphicsAsset::setWorldSize(const glm::dvec2& size)
{
    worldSize = size;
    markModified();
}

size_t VectorGraphicsAsset::getSizeInBytes() const
{
    size_t total = sizeof(VectorGraphicsAsset);
    if (vectorGraphics)
    {
        total += vectorGraphics->getByteSize();
    }
    return total;
}

#ifdef ISTESTPROJECT
#include "CADacomb/VectorGraphics/Svg/SvgReader.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("VectorGraphicsAsset/PropertiesAndAspectRatio")
{
    VectorGraphicsAsset asset;
    CHECK(asset.getType() == "VectorGraphics");

    auto doc = std::make_shared<CADacomb::VectorGraphics::VectorGraphics>(
      CADacomb::VectorGraphics::SvgReader::loadFromString(R"(<svg viewBox="0 0 200 100"><rect x="0" y="0" width="200" height="100"/></svg>)"));
    asset.setVectorGraphics(doc);

    CHECK_FALSE(asset.getVectorGraphics().isEmpty());
    CHECK(asset.getWorldSize().x == Catch::Approx(2.0));
    CHECK(asset.getWorldSize().y == Catch::Approx(1.0));
    CHECK(asset.getSizeInBytes() > sizeof(VectorGraphicsAsset));
}
#endif
