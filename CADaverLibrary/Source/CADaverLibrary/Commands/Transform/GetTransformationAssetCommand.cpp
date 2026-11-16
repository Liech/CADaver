#include "Commands/Transform/GetTransformationAssetCommand.h"

#include "Database/Asset.h"
#include "Database/Database.h"
#include "Database/Project.h"
#include "Utility/MatrixJson.h"

GetTransformationAssetCommand::GetTransformationAssetCommand() {}

GetTransformationAssetCommand::~GetTransformationAssetCommand() {}

std::string GetTransformationAssetCommand::getType() const
{
    return "GetTransformationAsset";
}

int GetTransformationAssetCommand::getVersion() const
{
    return 1;
}

std::string GetTransformationAssetCommand::getDescription() const
{
    return "Command: GetTransformationAsset\n"
           "Version: 1\n"
           "Purpose: Returns the 4x4 transformation matrix of an asset.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"GetTransformationAsset\",\n"
           "  \"version\": 1,\n"
           "  \"assetGuid\": <string>\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: None\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"GetTransformationAsset\",\n"
           "  \"version\": 1,\n"
           "  \"transformation\": [<double> x16, column-major]\n"
           "}";
}

void GetTransformationAssetCommand::setInput(const Polyglot::Node& node)
{
    assetGuid = CADacomb::GUID(node["assetGuid"].asString());
}

nlohmann::json GetTransformationAssetCommand::execute(Database& database)
{
    auto asset = database.getProject().getAsset(assetGuid);
    if (!asset)
    {
        return createErrorResponse("AssetNotFound", "Asset not found: " + assetGuid.toString());
    }

    return createResponse({
      { "transformation", MatrixJson::toJson(asset->getTransformation()) }
    });
}

#ifdef ISTESTPROJECT
#include "Database/Asset/TriangulationAsset.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("GetTransformationAssetCommand/ReturnsIdentityOrNotFound")
{
    Database database;
    auto     asset = std::make_shared<TriangulationAsset>();
    database.getProject().addAsset(asset);

    GetTransformationAssetCommand cmd;
    cmd.setInput(Polyglot::Node({
      { "assetGuid", asset->getGuid().toString() }
    }));
    nlohmann::json res = cmd.execute(database);
    CHECK(res.at("type").get<std::string>() == "GetTransformationAsset");
    CHECK(MatrixJson::fromJson(res.at("transformation")) == glm::dmat4(1.0));

    GetTransformationAssetCommand missing;
    missing.setInput(Polyglot::Node({
      { "assetGuid", CADacomb::GUID::random().toString() }
    }));
    nlohmann::json err = missing.execute(database);
    CHECK(err.at("type").get<std::string>() == "Error");
    CHECK(err.at("errorType").get<std::string>() == "AssetNotFound");
}
#endif
