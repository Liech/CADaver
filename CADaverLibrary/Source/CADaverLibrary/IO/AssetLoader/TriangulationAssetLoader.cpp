#include "IO/AssetLoader/TriangulationAssetLoader.h"

#include "CADacomb/Triangle/Triangulation.h"
#include "CADacomb/Util/ProgressBar.h"
#include "Database/Asset/TriangulationAsset.h"
#include "Utility/IOValueGrabber.h"

#include <sstream>
#include <vector>

TriangulationAssetLoader::TriangulationAssetLoader() {}

TriangulationAssetLoader::~TriangulationAssetLoader() {}

std::shared_ptr<Asset> TriangulationAssetLoader::load(const std::string& name, const Polyglot::Node& source) const
{
    CADacomb::ProgressBar bar(3, "Loading STL asset");

    std::unique_ptr<CADacomb::Triangulation> triangulation;

    std::vector<uint8_t> bytes = IOValueGrabber::grabBytes(source);
    bar.addProgress(1);

    if (!bytes.empty())
    {
        std::string        str(bytes.begin(), bytes.end());
        std::istringstream stream(str, std::ios::binary);
        triangulation = CADacomb::Triangulation::fromSTLStream(stream);
    }
    bar.addProgress(1);

    if (!triangulation || triangulation->indices.empty() || bar.isCanceled())
    {
        return nullptr;
    }

    auto asset = std::make_shared<TriangulationAsset>();
    asset->setName(name);
    asset->setTriangulation(std::move(triangulation));

    bar.addProgress(1);
    return asset;
}
