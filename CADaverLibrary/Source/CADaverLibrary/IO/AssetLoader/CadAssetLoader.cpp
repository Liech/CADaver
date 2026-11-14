#include "IO/AssetLoader/CadAssetLoader.h"

#include "CADacomb/CAD/Sm/KernelSm.h"
#include "CADacomb/Util/GUID.h"
#include "CADacomb/Util/ProgressBar.h"
#include "Database/Asset/CadAsset.h"
#include "Utility/IOValueGrabber.h"

#include <filesystem>
#include <fstream>
#include <vector>

CadAssetLoader::CadAssetLoader() {}

CadAssetLoader::~CadAssetLoader() {}

std::shared_ptr<Asset> CadAssetLoader::load(const std::string& name, const Polyglot::Node& source) const
{
    CADacomb::ProgressBar bar(3, "Loading STEP asset");

    CADacomb::CAD::KernelSm                            kernel;
    std::vector<std::unique_ptr<CADacomb::CAD::Shape>> shapes;

    if (IOValueGrabber::isFilePath(source))
    {
        bar.addProgress(1);
        shapes = kernel.loadStepFile(IOValueGrabber::grabFilePath(source));
    }
    else
    {
        std::vector<uint8_t> bytes = IOValueGrabber::grabBytes(source);
        bar.addProgress(1);
        if (!bytes.empty())
        {
            std::string tempPath = (std::filesystem::temp_directory_path() / ("cadaver_temp_" + CADacomb::GUID::random().toString() + ".step")).string();
            {
                std::ofstream tempOut(tempPath, std::ios::binary);
                std::string   str(bytes.begin(), bytes.end());
                tempOut.write(str.data(), static_cast<std::streamsize>(str.size()));
            }
            shapes = kernel.loadStepFile(tempPath);
            std::filesystem::remove(tempPath);
        }
    }
    bar.addProgress(1);

    if (shapes.empty() || !shapes[0] || bar.isCanceled())
    {
        return nullptr;
    }

    auto asset = std::make_shared<CadAsset>();
    asset->setName(name);
    asset->setCadShape(std::move(shapes[0]));

    bar.addProgress(1);
    return asset;
}
