#include "Commands/Selection/GetSelectedAssetsCommand.h"

#include "Database/Database.h"

GetSelectedAssetsCommand::GetSelectedAssetsCommand() {}

GetSelectedAssetsCommand::~GetSelectedAssetsCommand() {}

std::string GetSelectedAssetsCommand::getType() const
{
    return "GetSelectedAssets";
}

int GetSelectedAssetsCommand::getVersion() const
{
    return 1;
}

std::string GetSelectedAssetsCommand::getDescription() const
{
    return "Command: GetSelectedAssets\n"
           "Version: 1\n"
           "Purpose: Returns the list of currently selected asset GUIDs from the database.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"GetSelectedAssets\",\n"
           "  \"version\": 1\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: None\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"GetSelectedAssets\",\n"
           "  \"version\": 1,\n"
           "  \"status\": \"Success\",\n"
           "  \"assetGuids\": [<string>, ...]\n"
           "}";
}

void GetSelectedAssetsCommand::setInput(const Polyglot::Node& node) {}

nlohmann::json GetSelectedAssetsCommand::execute(Database& database)
{
    nlohmann::json guidsJson = nlohmann::json::array();
    for (const auto& entry : database.getSelection().getEntries())
    {
        guidsJson.push_back(entry.getTargetGuid().toString());
    }

    return createResponse("Success", {
        { "assetGuids", guidsJson }
    });
}

#ifdef ISTESTPROJECT
#include "Database/Selection/TriangleSubObjectSelection.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("GetSelectedAssetsCommand/ReturnsOnlyGuids")
{
    Database       database;
    CADacomb::GUID guidA = CADacomb::GUID::random();
    CADacomb::GUID guidB = CADacomb::GUID::random();

    auto triSub = std::make_shared<TriangleSubObjectSelection>();
    triSub->add(1);
    triSub->add(2);

    SelectionEntry entryA;
    entryA.setTargetGuid(guidA);
    entryA.addSubObject(triSub);
    database.getSelection().select(entryA);

    SelectionEntry entryB;
    entryB.setTargetGuid(guidB);
    database.getSelection().addSelection(entryB);

    GetSelectedAssetsCommand cmd;
    nlohmann::json           result = cmd.execute(database);

    CHECK(result.at("status").get<std::string>() == "Success");
    const auto& guids = result.at("assetGuids");
    REQUIRE(guids.size() == 2);
}
#endif
