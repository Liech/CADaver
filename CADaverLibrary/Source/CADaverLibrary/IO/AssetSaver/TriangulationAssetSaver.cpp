#include "IO/AssetSaver/TriangulationAssetSaver.h"

#include "CADacomb/Util/ProgressBar.h"
#include "Database/Asset/TriangulationAsset.h"
#include "Utility/IOValueGrabber.h"

#include <sstream>
#include <vector>

TriangulationAssetSaver::TriangulationAssetSaver() {}

TriangulationAssetSaver::~TriangulationAssetSaver() {}

bool TriangulationAssetSaver::save(const Asset& asset, const Polyglot::Node& destination) const
{
    CADacomb::ProgressBar bar(2, "Saving STL asset");
    if (bar.isCanceled())
    {
        return false;
    }

    const auto&        triAsset       = static_cast<const TriangulationAsset&>(asset);
    const glm::dmat4&  transformation = triAsset.getTransformation();
    std::ostringstream memStream(std::ios::binary);

    if (transformation == glm::dmat4(1.0))
    {
        triAsset.getTriangulation().saveAsSTL(memStream);
    }
    else
    {
        CADacomb::Triangulation transformed = triAsset.getTriangulation();
        for (auto& vertex : transformed.vertices)
        {
            vertex = glm::dvec3(transformation * glm::dvec4(vertex, 1.0));
        }
        transformed.saveAsSTL(memStream);
    }

    bar.addProgress(1);
    if (bar.isCanceled())
    {
        return false;
    }

    std::string          str = memStream.str();
    std::vector<uint8_t> bytes(str.begin(), str.end());
    if (!IOValueGrabber::deliverBytes(destination, bytes))
    {
        return false;
    }

    bar.addProgress(1);
    return true;
}

#ifdef ISTESTPROJECT
#include "IO/AssetLoader/TriangulationAssetLoader.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <glm/gtc/matrix_transform.hpp>

TEST_CASE("TriangulationAssetSaver/BakesTransformationIntoStl")
{
    auto tri      = std::make_unique<CADacomb::Triangulation>();
    tri->vertices = { glm::dvec3(0.0, 0.0, 0.0), glm::dvec3(1.0, 0.0, 0.0), glm::dvec3(0.0, 1.0, 0.0) };
    tri->indices  = { 0, 1, 2 };

    TriangulationAsset asset;
    asset.setTriangulation(std::move(tri));
    asset.setTransformation(glm::translate(glm::dmat4(1.0), glm::dvec3(10.0, 20.0, 30.0)));

    std::vector<uint8_t> savedBytes;
    Polyglot::Node       destCallback(
      std::function<Polyglot::Node(const Polyglot::Node&)>(
        [&savedBytes](const Polyglot::Node& data) -> Polyglot::Node
        {
            savedBytes = data.asBinary();
            return Polyglot::Node(true);
        }));

    TriangulationAssetSaver saver;
    REQUIRE(saver.save(asset, destCallback));
    REQUIRE_FALSE(savedBytes.empty());

    TriangulationAssetLoader loader;
    auto                     loaded = std::dynamic_pointer_cast<TriangulationAsset>(loader.load("TestMesh", Polyglot::Node(savedBytes)));
    REQUIRE(loaded != nullptr);
    REQUIRE(loaded->getTriangulation().vertices.size() >= 3);
    CHECK(loaded->getTriangulation().vertices[0].x == Catch::Approx(10.0).margin(1e-4));
    CHECK(loaded->getTriangulation().vertices[0].y == Catch::Approx(20.0).margin(1e-4));
    CHECK(loaded->getTriangulation().vertices[0].z == Catch::Approx(30.0).margin(1e-4));
}
#endif
