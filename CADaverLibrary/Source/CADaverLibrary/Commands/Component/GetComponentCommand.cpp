#include "Commands/Component/GetComponentCommand.h"

#include "Database/Asset.h"
#include "Database/AssetComponent.h"
#include "Database/Database.h"
#include "Database/Project.h"

GetComponentCommand::GetComponentCommand() {}

GetComponentCommand::~GetComponentCommand() {}

std::string GetComponentCommand::getType() const
{
    return "GetComponent";
}

int GetComponentCommand::getVersion() const
{
    return 1;
}

std::string GetComponentCommand::getDescription() const
{
    return "Command: GetComponent\n"
           "Version: 1\n"
           "Purpose: Retrieves one or all components attached to an asset, optionally filtered by componentGuid or componentType.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"GetComponent\",\n"
           "  \"version\": 1,\n"
           "  \"assetGuid\": \"<string>\",\n"
           "  \"componentGuid\": \"<string, optional>\",\n"
           "  \"componentType\": \"<string, optional>\"\n"
           "}\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"GetComponent\",\n"
           "  \"version\": 1,\n"
           "  \"components\": [ <componentJson>, ... ]\n"
           "}";
}

void GetComponentCommand::setInput(const Polyglot::Node& node)
{
    assetGuid     = CADacomb::GUID(node["assetGuid"].asString());
    componentGuid = node.contains("componentGuid") ? CADacomb::GUID(node["componentGuid"].asString()) : CADacomb::GUID();
    componentType = node.contains("componentType") ? node["componentType"].asString() : "";
}

nlohmann::json GetComponentCommand::execute(Database& database)
{
    auto asset = database.getProject().getAsset(assetGuid);
    if (!asset)
    {
        return createErrorResponse("AssetNotFound", "Asset not found: " + assetGuid.toString());
    }

    nlohmann::json resultList = nlohmann::json::array();

    if (!componentGuid.toString().empty())
    {
        auto component = asset->getComponent(componentGuid);
        if (!component)
        {
            return createErrorResponse("ComponentNotFound", "Component not found: " + componentGuid.toString());
        }
        resultList.push_back(component->toJson());
    }
    else
    {
        for (const auto& component : asset->getAttachments())
        {
            if (componentType.empty() || component->getType() == componentType)
            {
                resultList.push_back(component->toJson());
            }
        }
    }

    return createResponse({
        { "components", resultList }
    });
}

#ifdef ISTESTPROJECT
#include "Database/Asset/TriangulationAsset.h"
#include "Database/Component/FaceValuesComponent.h"
#include "Database/Component/TextNoteComponent.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("GetComponentCommand/FiltersByTypeAndGuid")
{
    Database database;
    auto     triAsset = std::make_shared<TriangulationAsset>();
    auto     note     = std::make_shared<TextNoteComponent>("Hello");
    auto     faces    = std::make_shared<FaceValuesComponent>();
    triAsset->addComponent(note);
    triAsset->addComponent(faces);
    database.getProject().addAsset(triAsset);

    GetComponentCommand getAll;
    getAll.setInput(Polyglot::Node({
      { "assetGuid", triAsset->getGuid().toString() }
    }));
    nlohmann::json allRes = getAll.execute(database);
    CHECK(allRes.at("components").size() == 2);

    GetComponentCommand getNotes;
    getNotes.setInput(Polyglot::Node({
      {     "assetGuid", triAsset->getGuid().toString() },
      { "componentType",                   "TextNote" }
    }));
    nlohmann::json noteRes = getNotes.execute(database);
    REQUIRE(noteRes.at("components").size() == 1);
    CHECK(noteRes.at("components")[0].at("type").get<std::string>() == "TextNote");

    GetComponentCommand getOne;
    getOne.setInput(Polyglot::Node({
      {     "assetGuid", triAsset->getGuid().toString() },
      { "componentGuid",    faces->getGuid().toString() }
    }));
    nlohmann::json oneRes = getOne.execute(database);
    REQUIRE(oneRes.at("components").size() == 1);
    CHECK(oneRes.at("components")[0].at("type").get<std::string>() == "FaceValues");
}
#endif
