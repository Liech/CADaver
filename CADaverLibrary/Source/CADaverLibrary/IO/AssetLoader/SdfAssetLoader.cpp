#include "IO/AssetLoader/SdfAssetLoader.h"

#include "CADacomb/Formula/Formula.h"
#include "Database/Asset/SdfAsset.h"
#include "Utility/IOValueGrabber.h"
#include "Utility/MatrixJson.h"

#include <nlohmann/json.hpp>
#include <stdexcept>
#include <vector>

SdfAssetLoader::SdfAssetLoader() {}

SdfAssetLoader::~SdfAssetLoader() {}

std::shared_ptr<Asset> SdfAssetLoader::load(const std::string& name, const Polyglot::Node& source) const
{
    std::vector<uint8_t> bytes = IOValueGrabber::grabBytes(source);
    if (bytes.empty())
    {
        return nullptr;
    }

    nlohmann::json json    = nlohmann::json::parse(bytes.begin(), bytes.end());
    int            version = json.at("Version").get<int>();
    if (version != 1)
    {
        throw std::runtime_error("Unsupported SdfAsset file version");
    }

    CADacomb::Formula formula = CADacomb::Formula::fromJson(json.at("Formula"));

    const nlohmann::json& minJson = json.at("BoundsMin");
    const nlohmann::json& maxJson = json.at("BoundsMax");
    CADacomb::AABB3       bounds(glm::dvec3(minJson.at(0).get<double>(), minJson.at(1).get<double>(), minJson.at(2).get<double>()),
                                 glm::dvec3(maxJson.at(0).get<double>(), maxJson.at(1).get<double>(), maxJson.at(2).get<double>()));

    glm::dmat4 transformation = MatrixJson::fromJson(json.at("Transformation"));

    auto asset = std::make_shared<SdfAsset>();
    asset->setName(name);
    asset->setFormula(formula);
    asset->setAABB(bounds);
    asset->setTransformation(transformation);
    return asset;
}
