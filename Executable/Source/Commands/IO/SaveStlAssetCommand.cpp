#include "Commands/IO/SaveStlAssetCommand.h"

#include "CADacomb/Util/ProgressBar.h"
#include "Database/Asset/TriangulationAsset.h"
#include "Database/Database.h"

#include <fstream>

SaveStlAssetCommand::SaveStlAssetCommand() {}

SaveStlAssetCommand::SaveStlAssetCommand(const CADacomb::GUID& assetGuid, const std::string& filePath)
  : filePath(filePath)
  , assetGuid(assetGuid)
{
}

SaveStlAssetCommand::SaveStlAssetCommand(const nlohmann::json& json)
  : filePath(json.value("filePath", ""))
  , assetGuid(json.value("assetGuid", ""))
{
}

SaveStlAssetCommand::~SaveStlAssetCommand() {}

std::string SaveStlAssetCommand::getType() const
{
    return "SaveStlAsset";
}

int SaveStlAssetCommand::getVersion() const
{
    return 1;
}

std::string SaveStlAssetCommand::getDescription() const
{
    return "Command: SaveStlAsset\n"
           "Version: 1\n"
           "Purpose: Saves a triangulation asset to a binary STL output stream or file path.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"SaveStlAsset\",\n"
           "  \"version\": 1,\n"
           "  \"assetGuid\": <string>,\n"
           "  \"filePath\": <string> (optional)\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: Binary STL data (optional if filePath specified)\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"SaveStlAsset\",\n"
           "  \"version\": 1,\n"
           "  \"status\": \"Success\" | \"AssetNotFound\" | \"NotTriangulationAsset\" | \"NoTriangulation\" | \"NoOutputStream\"\n"
           "}";
}

std::vector<CADacomb::GUID> SaveStlAssetCommand::getRequiredAssets() const
{
    return { assetGuid };
}

nlohmann::json SaveStlAssetCommand::execute(Database& database, std::istream*, std::ostream* outputStream, bool)
{
    CADacomb::ProgressBar bar(1, "Saving STL asset");

    auto baseAsset = database.getProject().getAsset(assetGuid);
    if (!baseAsset || bar.isCanceled())
    {
        return createResponse("AssetNotFound");
    }

    auto triAsset = std::dynamic_pointer_cast<TriangulationAsset>(baseAsset);
    if (!triAsset)
    {
        return createResponse("NotTriangulationAsset");
    }

    auto* tri = triAsset->getTriangulation();
    if (!tri)
    {
        return createResponse("NoTriangulation");
    }

    std::ofstream fileStream;
    std::ostream* streamToUse = outputStream;
    if (!streamToUse && !filePath.empty())
    {
        fileStream.open(filePath, std::ios::binary);
        if (fileStream.is_open())
        {
            streamToUse = &fileStream;
        }
    }

    if (!streamToUse || bar.isCanceled())
    {
        return createResponse("NoOutputStream");
    }

    tri->saveAsSTL(*streamToUse);
    bar.addProgress(1);

    return createResponse("Success");
}

const CADacomb::GUID& SaveStlAssetCommand::getAssetGuid() const
{
    return assetGuid;
}

void SaveStlAssetCommand::setAssetGuid(const CADacomb::GUID& guid)
{
    assetGuid = guid;
}

const std::string& SaveStlAssetCommand::getFilePath() const
{
    return filePath;
}

void SaveStlAssetCommand::setFilePath(const std::string& newFilePath)
{
    filePath = newFilePath;
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("SaveStlAssetCommand/TypeAndVersion")
{
    CADacomb::GUID      guid = CADacomb::GUID::random();
    SaveStlAssetCommand cmd(guid, "output.stl");
    CHECK(cmd.getType() == "SaveStlAsset");
    CHECK(cmd.getVersion() == 1);
    CHECK_FALSE(cmd.getDescription().empty());
    CHECK(cmd.getAssetGuid() == guid);
    CHECK(cmd.getFilePath() == "output.stl");
    REQUIRE(cmd.getRequiredAssets().size() == 1);
    CHECK(cmd.getRequiredAssets().front() == guid);
}
#endif
