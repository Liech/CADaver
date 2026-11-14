#include "IO/AssetSaver/CadAssetSaver.h"

#include "CADacomb/CAD/Sm/KernelSm.h"
#include "CADacomb/Util/GUID.h"
#include "CADacomb/Util/ProgressBar.h"
#include "Database/Asset/CadAsset.h"
#include "Utility/IOValueGrabber.h"

#include <filesystem>
#include <fstream>
#include <vector>

CadAssetSaver::CadAssetSaver() {}

CadAssetSaver::~CadAssetSaver() {}

bool CadAssetSaver::save(const Asset& asset, const Polyglot::Node& destination) const
{
    CADacomb::ProgressBar bar(2, "Saving STEP asset");
    if (bar.isCanceled())
    {
        return false;
    }

    const auto&             cadAsset = static_cast<const CadAsset&>(asset);
    CADacomb::CAD::KernelSm kernel;

    if (IOValueGrabber::isFunction(destination))
    {
        std::string tempPath = (std::filesystem::temp_directory_path() / ("cadaver_temp_" + CADacomb::GUID::random().toString() + ".step")).string();
        kernel.saveStepFile(cadAsset.getCadShape(), tempPath, cadAsset.getName());
        bar.addProgress(1);
        if (bar.isCanceled())
        {
            std::filesystem::remove(tempPath);
            return false;
        }

        std::vector<uint8_t> bytes;
        {
            std::ifstream tempIn(tempPath, std::ios::binary | std::ios::ate);
            if (tempIn.is_open())
            {
                std::streamsize size = tempIn.tellg();
                tempIn.seekg(0, std::ios::beg);
                std::vector<char> buffer(static_cast<size_t>(size));
                tempIn.read(buffer.data(), size);
                bytes.assign(buffer.begin(), buffer.end());
            }
        }
        std::filesystem::remove(tempPath);
        if (!IOValueGrabber::deliverBytes(destination, bytes))
        {
            return false;
        }
        bar.addProgress(1);
        return true;
    }

    if (IOValueGrabber::isFilePath(destination))
    {
        std::string targetPath = IOValueGrabber::grabFilePath(destination);
        if (targetPath.empty())
        {
            targetPath = cadAsset.getName() + ".step";
        }
        kernel.saveStepFile(cadAsset.getCadShape(), targetPath, cadAsset.getName());
        bar.addProgress(1);
        if (bar.isCanceled())
        {
            return false;
        }
        bar.addProgress(1);
        return true;
    }

    return false;
}
