#include "Commands/IO/SaveStepAssetCommand.h"

#include "CADacomb/CAD/Sm/KernelSm.h"
#include "CADacomb/Util/ProgressBar.h"
#include "Database/Asset/CadAsset.h"
#include "Database/Database.h"

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
           "Purpose: Saves a CAD asset to a STEP file.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"SaveStepAsset\",\n"
           "  \"version\": 1,\n"
           "  \"assetGuid\": <string>,\n"
           "  \"filePath\": <string> (optional)\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: None\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"SaveStepAsset\",\n"
           "  \"version\": 1,\n"
           "  \"status\": \"Success\" | \"AssetNotFound\" | \"NotCadAsset\" | \"NoShape\" | \"WriteFailed\",\n"
           "  \"filePath\": <string>\n"
           "}";
}

std::vector<CADacomb::GUID> SaveStepAssetCommand::getRequiredAssets() const
{
    return { assetGuid };
}

void SaveStepAssetCommand::setInput(const nlohmann::json& json)
{
    filePath  = json.value("filePath", "");
    assetGuid = json.value("assetGuid", "");
}

nlohmann::json SaveStepAssetCommand::execute(Database& database, std::istream*, std::ostream*)
{
    CADacomb::ProgressBar bar(1, "Saving STEP asset");

    auto baseAsset = database.getProject().getAsset(assetGuid);
    if (!baseAsset || bar.isCanceled())
    {
        return createResponse("AssetNotFound",
                              {
                                { "filePath", filePath }
        });
    }

    auto cadAsset = std::dynamic_pointer_cast<CadAsset>(baseAsset);
    if (!cadAsset)
    {
        return createResponse("NotCadAsset",
                              {
                                { "filePath", filePath }
        });
    }

    std::string targetPath = filePath;
    if (targetPath.empty())
    {
        targetPath = cadAsset->getName() + ".step";
    }

    CADacomb::CAD::KernelSm kernel;
    kernel.saveStepFile(cadAsset->getCadShape(), targetPath, cadAsset->getName());

    if (bar.isCanceled())
    {
        return createResponse("WriteFailed",
                              {
                                { "filePath", targetPath }
        });
    }

    bar.addProgress(1);

    return createResponse("Success",
                          {
                            { "filePath", targetPath }
    });
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("SaveStepAssetCommand/TypeAndVersion")
{
    CADacomb::GUID       guid = CADacomb::GUID::random();
    SaveStepAssetCommand cmd;
    cmd.setInput({
      { "assetGuid", guid.toString() },
      {  "filePath",   "output.step" }
    });
    CHECK(cmd.getType() == "SaveStepAsset");
    CHECK(cmd.getVersion() == 1);
    CHECK_FALSE(cmd.getDescription().empty());
    REQUIRE(cmd.getRequiredAssets().size() == 1);
    CHECK(cmd.getRequiredAssets().front() == guid);
}
#endif
