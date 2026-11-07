#include "Commands/IO/SaveStepAssetCommand.h"

#include "CADacomb/CAD/Sm/KernelSm.h"
#include "CADacomb/Util/ProgressBar.h"
#include "Database/Asset/CadAsset.h"
#include "Database/Database.h"
#include "Utility/IOValueGrabber.h"

#include <filesystem>
#include <fstream>

SaveStepAssetCommand::SaveStepAssetCommand() {}

SaveStepAssetCommand::~SaveStepAssetCommand() {}

std::string SaveStepAssetCommand::getType() const
{
    return "SaveStepAsset";
}

int SaveStepAssetCommand::getVersion() const
{
    return 1;
}

std::string SaveStepAssetCommand::getDescription() const
{
    return "Command: SaveStepAsset\n"
           "Version: 1\n"
           "Purpose: Saves a CAD asset to a destination callback or STEP file.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"SaveStepAsset\",\n"
           "  \"version\": 1,\n"
           "  \"assetGuid\": <string>,\n"
           "  \"destination\": <callback or string>\n"
           "}\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"SaveStepAsset\",\n"
           "  \"version\": 1,\n"
           "  \"status\": \"Success\" | \"AssetNotFound\" | \"NotCadAsset\" | \"WriteFailed\"\n"
           "}";
}

std::vector<CADacomb::GUID> SaveStepAssetCommand::getRequiredAssets() const
{
    return { assetGuid };
}

void SaveStepAssetCommand::setInput(const Polyglot::Node& node)
{
    assetGuid   = node["assetGuid"].asString();
    destination = node["destination"];
}

nlohmann::json SaveStepAssetCommand::execute(Database& database)
{
    CADacomb::ProgressBar bar(2, "Saving STEP asset");

    auto baseAsset = database.getProject().getAsset(assetGuid);
    if (!baseAsset || bar.isCanceled())
    {
        return createResponse("AssetNotFound");
    }

    auto cadAsset = std::dynamic_pointer_cast<CadAsset>(baseAsset);
    if (!cadAsset)
    {
        return createResponse("NotCadAsset");
    }

    CADacomb::CAD::KernelSm kernel;

    if (IOValueGrabber::isFunction(destination))
    {
        std::string tempPath = (std::filesystem::temp_directory_path() / ("cadaver_temp_" + CADacomb::GUID::random().toString() + ".step")).string();
        kernel.saveStepFile(cadAsset->getCadShape(), tempPath, cadAsset->getName());
        bar.addProgress(1);
        if (bar.isCanceled())
        {
            std::filesystem::remove(tempPath);
            return createResponse("WriteFailed");
        }

        std::vector<uint8_t> bytes;
        {
            std::ifstream tempIn(tempPath, std::ios::binary | std::ios::ate);
            if (tempIn.is_open())
            {
                std::streamsize size = tempIn.tellg();
                tempIn.seekg(0, std::ios::beg);
                bytes.resize(static_cast<size_t>(size));
                tempIn.read(reinterpret_cast<char*>(bytes.data()), size);
            }
        }
        std::filesystem::remove(tempPath);
        if (!IOValueGrabber::deliverBytes(destination, bytes))
        {
            return createResponse("WriteFailed");
        }
        bar.addProgress(1);
        return createResponse("Success");
    }
    else if (IOValueGrabber::isFilePath(destination))
    {
        std::string targetPath = IOValueGrabber::grabFilePath(destination);
        if (targetPath.empty())
        {
            targetPath = cadAsset->getName() + ".step";
        }
        kernel.saveStepFile(cadAsset->getCadShape(), targetPath, cadAsset->getName());
        bar.addProgress(1);
        if (bar.isCanceled())
        {
            return createResponse("WriteFailed");
        }
        bar.addProgress(1);
        return createResponse("Success",
                              {
                                { "filePath", targetPath }
        });
    }

    return createResponse("WriteFailed");
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("SaveStepAssetCommand/TypeAndVersion")
{
    CADacomb::GUID       guid = CADacomb::GUID::random();
    SaveStepAssetCommand cmd;
    cmd.setInput(Polyglot::Node({
      {   "assetGuid", guid.toString() },
      { "destination",   "output.step" }
    }));
    CHECK(cmd.getType() == "SaveStepAsset");
    CHECK(cmd.getVersion() == 1);
    CHECK_FALSE(cmd.getDescription().empty());
    REQUIRE(cmd.getRequiredAssets().size() == 1);
    CHECK(cmd.getRequiredAssets().front() == guid);
}
#endif
