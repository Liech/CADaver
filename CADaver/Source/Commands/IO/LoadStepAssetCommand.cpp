#include "Commands/IO/LoadStepAssetCommand.h"

#include "CADacomb/CAD/Sm/KernelSm.h"
#include "CADacomb/Util/ProgressBar.h"
#include "Commands/Undo/AddAssetUndoRecord.h"
#include "Database/Asset/CadAsset.h"
#include "Database/Database.h"

LoadStepAssetCommand::LoadStepAssetCommand() {}

LoadStepAssetCommand::LoadStepAssetCommand(const std::string& name, const std::string& filePath)
  : name(name)
  , filePath(filePath)
{
}

LoadStepAssetCommand::LoadStepAssetCommand(const nlohmann::json& json)
  : name(json.at("name").get<std::string>())
  , filePath(json.at("filePath").get<std::string>())
{
}

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

nlohmann::json LoadStepAssetCommand::execute(Database& database, std::istream*, std::ostream*, bool recordUndo)
{
    CADacomb::ProgressBar bar(1, "Loading STEP asset");

    CADacomb::CAD::KernelSm                            kernel;
    std::vector<std::unique_ptr<CADacomb::CAD::Shape>> shapes;
    try
    {
        shapes = kernel.loadStepFile(filePath);
    }
    catch (...)
    {
        return createResponse("LoadFailed",
                              {
                                { "assetGuid",   "" },
                                {      "name", name }
        });
    }

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

    if (recordUndo)
    {
        database.getUndoStack().push(std::make_unique<AddAssetUndoRecord>(asset));
    }

    return createResponse("Success",
                          {
                            { "assetGuid", asset->getGuid().toString() },
                            {      "name",                        name }
    });
}

const std::string& LoadStepAssetCommand::getName() const
{
    return name;
}

void LoadStepAssetCommand::setName(const std::string& newName)
{
    name = newName;
}

const std::string& LoadStepAssetCommand::getFilePath() const
{
    return filePath;
}

void LoadStepAssetCommand::setFilePath(const std::string& newFilePath)
{
    filePath = newFilePath;
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("LoadStepAssetCommand/TypeAndVersion")
{
    LoadStepAssetCommand cmd("Part", "part.step");
    CHECK(cmd.getType() == "LoadStepAsset");
    CHECK(cmd.getVersion() == 1);
    CHECK_FALSE(cmd.getDescription().empty());
    CHECK(cmd.getName() == "Part");
    CHECK(cmd.getFilePath() == "part.step");
    CHECK(cmd.getRequiredAssets().empty());
}
#endif
