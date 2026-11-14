#include "Commands/Selection/GetAssetSelectionCommand.h"

#include "Database/Database.h"

GetAssetSelectionCommand::GetAssetSelectionCommand() {}

GetAssetSelectionCommand::~GetAssetSelectionCommand() {}

std::string GetAssetSelectionCommand::getType() const
{
    return "GetAssetSelection";
}

int GetAssetSelectionCommand::getVersion() const
{
    return 1;
}

std::string GetAssetSelectionCommand::getDescription() const
{
    return "Command: GetAssetSelection\n"
           "Version: 1\n"
           "Purpose: Returns the selection entry and subobjects for a specific asset GUID.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"GetAssetSelection\",\n"
           "  \"version\": 1,\n"
           "  \"assetGuid\": <string>\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: None\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"GetAssetSelection\",\n"
           "  \"version\": 1,\n"
           "  \"status\": \"Success\" | \"NotSelected\",\n"
           "  \"selection\": <object>\n"
           "}";
}

void GetAssetSelectionCommand::setInput(const Polyglot::Node& node)
{
    assetGuid = CADacomb::GUID(node["assetGuid"].asString());
}

nlohmann::json GetAssetSelectionCommand::execute(Database& database)
{
    for (const auto& entry : database.getSelection().getEntries())
    {
        if (entry.getTargetGuid() == assetGuid)
        {
            return createResponse("Success", {
                { "selection", entry.toJson() }
            });
        }
    }

    return createResponse("NotSelected", {
        { "assetGuid", assetGuid.toString() }
    });
}

#ifdef ISTESTPROJECT
#include "Database/Selection/TriangleSubObjectSelection.h"
#include "Polyglot/Node.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("GetAssetSelectionCommand/ReturnsEntryForGuidOrNotSelected")
{
    Database       database;
    CADacomb::GUID guidA = CADacomb::GUID::random();
    CADacomb::GUID guidB = CADacomb::GUID::random();

    auto triSub = std::make_shared<TriangleSubObjectSelection>();
    triSub->add(42);

    SelectionEntry entryA;
    entryA.setTargetGuid(guidA);
    entryA.addSubObject(triSub);
    database.getSelection().select(entryA);

    GetAssetSelectionCommand cmdA;
    cmdA.setInput(Polyglot::Node({
      { "assetGuid", guidA.toString() }
    }));
    nlohmann::json resA = cmdA.execute(database);
    CHECK(resA.at("status").get<std::string>() == "Success");
    CHECK(resA.at("selection").at("targetGuid").get<std::string>() == guidA.toString());
    REQUIRE(resA.at("selection").at("subObjects").size() == 1);

    GetAssetSelectionCommand cmdB;
    cmdB.setInput(Polyglot::Node({
      { "assetGuid", guidB.toString() }
    }));
    nlohmann::json resB = cmdB.execute(database);
    CHECK(resB.at("status").get<std::string>() == "NotSelected");
}
#endif
