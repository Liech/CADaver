#include "IO/AssetSaver/SdfAssetSaver.h"

#include "Database/Asset/SdfAsset.h"
#include "Utility/IOValueGrabber.h"
#include "Utility/MatrixJson.h"

#include <nlohmann/json.hpp>
#include <vector>

SdfAssetSaver::SdfAssetSaver() {}

SdfAssetSaver::~SdfAssetSaver() {}

bool SdfAssetSaver::save(const Asset& asset, const Polyglot::Node& destination) const
{
    const auto& sdfAsset = static_cast<const SdfAsset&>(asset);
    const auto& bounds   = sdfAsset.getAABB();

    nlohmann::json json;
    json["Version"]        = 1;
    json["Formula"]        = sdfAsset.getFormula().toJson();
    json["BoundsMin"]      = { bounds.getMin().x, bounds.getMin().y, bounds.getMin().z };
    json["BoundsMax"]      = { bounds.getMax().x, bounds.getMax().y, bounds.getMax().z };
    json["Transformation"] = MatrixJson::toJson(sdfAsset.getTransformation());

    std::string          serialized = json.dump(2);
    std::vector<uint8_t> bytes(serialized.begin(), serialized.end());
    return IOValueGrabber::deliverBytes(destination, bytes);
}

#ifdef ISTESTPROJECT
#include "IO/AssetLoader/SdfAssetLoader.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("SdfAssetSaver/RoundTripFormulaAndBounds")
{
    SdfAsset          asset;
    CADacomb::Formula formula;
    formula.setName("torusSdf");
    formula.setInputCount(3);
    formula.setOutputCount(1);
    formula.setHelper("float sdSphere(float3 p, float r) { return length(p) - r; }");
    formula.setExpression("output[0] = sdSphere(float3(input[0], input[1], input[2]), 0.4);");
    asset.setFormula(formula);
    asset.setAABB(CADacomb::AABB3(glm::dvec3(-2.0, -1.0, -3.0), glm::dvec3(2.0, 1.0, 3.0)));

    std::vector<uint8_t> savedBytes;
    Polyglot::Node       writer(
      [&savedBytes](const Polyglot::Node& data) -> Polyglot::Node
      {
          savedBytes = data.asBinary();
          return Polyglot::Node(true);
      });

    SdfAssetSaver saver;
    REQUIRE(saver.save(asset, writer));
    REQUIRE_FALSE(savedBytes.empty());

    SdfAssetLoader loader;
    auto           loadedBase = loader.load("LoadedSdf", Polyglot::Node(savedBytes));
    REQUIRE(loadedBase != nullptr);

    auto loaded = std::dynamic_pointer_cast<SdfAsset>(loadedBase);
    REQUIRE(loaded != nullptr);
    CHECK(loaded->getName() == "LoadedSdf");
    CHECK(loaded->getFormula().getName() == "torusSdf");
    CHECK(loaded->getFormula().getHelper() == formula.getHelper());
    CHECK(loaded->getFormula().getExpression() == formula.getExpression());
    CHECK(loaded->getAABB().getMin().x == Catch::Approx(-2.0));
    CHECK(loaded->getAABB().getMax().z == Catch::Approx(3.0));
}
#endif
