#include "Commands/IO/SaveStlAssetCommand.h"

#include "CADacomb/Util/ProgressBar.h"
#include "Database/Asset/TriangulationAsset.h"
#include "Database/Database.h"
#include "Utility/IOValueGrabber.h"

#include <sstream>

SaveStlAssetCommand::SaveStlAssetCommand() {}

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
           "Purpose: Saves a triangulation asset to a destination callback or file path.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"SaveStlAsset\",\n"
           "  \"version\": 1,\n"
           "  \"assetGuid\": <string>,\n"
           "  \"destination\": <callback or string>\n"
           "}\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"SaveStlAsset\",\n"
           "  \"version\": 1,\n"
           "  \"status\": \"Success\" | \"AssetNotFound\" | \"NotTriangulationAsset\" | \"NoTriangulation\"\n"
           "}";
}

std::vector<CADacomb::GUID> SaveStlAssetCommand::getRequiredAssets() const
{
    return { assetGuid };
}

void SaveStlAssetCommand::setInput(const Polyglot::Node& node)
{
    auto& mutableNode = const_cast<Polyglot::Node&>(node);
    assetGuid         = static_cast<std::string>(mutableNode["assetGuid"]);
    destination       = mutableNode["destination"];
}

nlohmann::json SaveStlAssetCommand::execute(Database& database)
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

    std::ostringstream memStream(std::ios::binary);
    triAsset->getTriangulation().saveAsSTL(memStream);
    std::string          str = memStream.str();
    std::vector<uint8_t> bytes(str.begin(), str.end());

    if (!IOValueGrabber::deliverBytes(destination, bytes))
    {
        return createResponse("WriteFailed");
    }

    bar.addProgress(1);

    return createResponse("Success");
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("SaveStlAssetCommand/TypeAndVersion")
{
    CADacomb::GUID      guid = CADacomb::GUID::random();
    SaveStlAssetCommand cmd;
    cmd.setInput(Polyglot::Node({
      {   "assetGuid", guid.toString() },
      { "destination",    "output.stl" }
    }));
    CHECK(cmd.getType() == "SaveStlAsset");
    CHECK(cmd.getVersion() == 1);
    CHECK_FALSE(cmd.getDescription().empty());
    REQUIRE(cmd.getRequiredAssets().size() == 1);
    CHECK(cmd.getRequiredAssets().front() == guid);
}
#endif
