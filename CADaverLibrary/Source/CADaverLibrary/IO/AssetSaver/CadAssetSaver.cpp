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

    const auto&             cadAsset       = static_cast<const CadAsset&>(asset);
    const glm::dmat4&       transformation = cadAsset.getTransformation();
    CADacomb::CAD::KernelSm kernel;

    if (IOValueGrabber::isFunction(destination))
    {
        std::string tempPath = (std::filesystem::temp_directory_path() / ("cadaver_temp_" + CADacomb::GUID::random().toString() + ".step")).string();
        kernel.saveStepFile(cadAsset.getCadShape(), tempPath, cadAsset.getName(), transformation);
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
        kernel.saveStepFile(cadAsset.getCadShape(), targetPath, cadAsset.getName(), transformation);
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

#ifdef ISTESTPROJECT
#include "IO/AssetLoader/CadAssetLoader.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <glm/gtc/matrix_transform.hpp>

TEST_CASE("CadAssetSaver/BakesTransformationIntoStepWithoutMutatingAsset")
{
    CADacomb::CAD::KernelSm kernel;
    auto                    box = kernel.createBox(glm::dvec3(0.0, 0.0, 0.0), glm::dvec3(2.0, 2.0, 2.0));

    CadAsset asset;
    asset.setName("Box");
    asset.setCadShape(std::shared_ptr<CADacomb::CAD::Shape>(std::move(box)));
    asset.setTransformation(glm::translate(glm::dmat4(1.0), glm::dvec3(10.0, 20.0, 30.0)));

    std::vector<uint8_t> savedBytes;
    Polyglot::Node       destCallback(std::function<Polyglot::Node(const Polyglot::Node&)>(
      [&savedBytes](const Polyglot::Node& data) -> Polyglot::Node
      {
          savedBytes = data.asBinary();
          return Polyglot::Node(true);
      }));

    CadAssetSaver saver;
    REQUIRE(saver.save(asset, destCallback));
    REQUIRE_FALSE(savedBytes.empty());

    // Source shape remains in local coordinates.
    CADacomb::AABB3 localBounds = asset.getCadShape().getAABB();
    CHECK(localBounds.getMin().x == Catch::Approx(0.0).margin(1e-4));

    CadAssetLoader loader;
    auto           loaded = std::dynamic_pointer_cast<CadAsset>(loader.load("LoadedBox", Polyglot::Node(savedBytes)));
    REQUIRE(loaded != nullptr);
    CADacomb::AABB3 exportedBounds = loaded->getCadShape().getAABB();
    CHECK(exportedBounds.getMin().x == Catch::Approx(10.0).margin(1e-4));
    CHECK(exportedBounds.getMin().y == Catch::Approx(20.0).margin(1e-4));
    CHECK(exportedBounds.getMin().z == Catch::Approx(30.0).margin(1e-4));
}
#endif
