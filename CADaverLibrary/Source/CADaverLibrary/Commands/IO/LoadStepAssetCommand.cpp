#include "Commands/IO/LoadStepAssetCommand.h"

#include "CADacomb/CAD/Sm/KernelSm.h"
#include "CADacomb/Util/ProgressBar.h"
#include "Database/Asset/CadAsset.h"
#include "Database/Database.h"

LoadStepAssetCommand::LoadStepAssetCommand() {}

LoadStepAssetCommand::~LoadStepAssetCommand() {}

std::string LoadStepAssetCommand::getType() const
{
    return "LoadStepAsset";
}

int LoadStepAssetCommand::getVersion() const
{
    return 1;
}

std::string LoadStepAssetCommand::getDescription() const
{
    return "Command: LoadStepAsset\n"
           "Version: 1\n"
           "Purpose: Loads a STEP CAD asset from a file path into the project.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"LoadStepAsset\",\n"
           "  \"version\": 1,\n"
           "  \"name\": \"<string>\",\n"
           "  \"filePath\": \"<string>\"\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: None\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"LoadStepAsset\",\n"
           "  \"version\": 1,\n"
           "  \"status\": \"Success\",\n"
           "  \"index\": <int>,\n"
           "  \"name\": \"<string>\"\n"
           "}";
}

void LoadStepAssetCommand::setInput(const nlohmann::json& json)
{
    name     = json.at("name").get<std::string>();
    filePath = json.at("filePath").get<std::string>();
}

nlohmann::json LoadStepAssetCommand::execute(Database& database, std::istream*, std::ostream*)
{
    CADacomb::ProgressBar bar(1, "Loading STEP asset");

    CADacomb::CAD::KernelSm                            kernel;
    std::vector<std::unique_ptr<CADacomb::CAD::Shape>> shapes = kernel.loadStepFile(filePath);

    if (shapes.empty() || !shapes[0] || bar.isCanceled())
    {
        return createResponse("LoadFailed",
                              {
                                { "assetGuid",   "" },
                                {      "name", name }
        });
    }

    auto asset = std::make_shared<CadAsset>();
    asset->setName(name);
    asset->setCadShape(std::move(shapes[0]));

    bar.addProgress(1);

    database.getProject().addAsset(asset);
    createdAsset = asset;

    return createResponse("Success",
                          {
                            { "assetGuid", asset->getGuid().toString() },
                            {      "name",                        name }
    });
}

bool LoadStepAssetCommand::isUndoable() const
{
    return createdAsset != nullptr;
}

void LoadStepAssetCommand::undo(Database& database)
{
    if (createdAsset)
    {
        database.getProject().removeAsset(createdAsset->getGuid());
    }
}

void LoadStepAssetCommand::redo(Database& database)
{
    if (createdAsset)
    {
        database.getProject().addAsset(createdAsset);
    }
}

size_t LoadStepAssetCommand::getUndoSize() const
{
    return sizeof(LoadStepAssetCommand) + (createdAsset ? createdAsset->getSizeInBytes() : 0);
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("LoadStepAssetCommand/TypeAndVersion")
{
    LoadStepAssetCommand cmd;
    cmd.setInput({
      {     "name",      "Part" },
      { "filePath", "part.step" }
    });
    CHECK(cmd.getType() == "LoadStepAsset");
    CHECK(cmd.getVersion() == 1);
    CHECK_FALSE(cmd.getDescription().empty());
    CHECK(cmd.getRequiredAssets().empty());
}
#endif
