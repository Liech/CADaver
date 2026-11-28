#include "Commands/Generator/UpdateSdfAssetCommand.h"

#include "CADacomb/Formula/FormulaLuaEvaluator.h"
#include "Database/Asset/SdfAsset.h"
#include "Database/Database.h"

UpdateSdfAssetCommand::UpdateSdfAssetCommand() {}

UpdateSdfAssetCommand::~UpdateSdfAssetCommand() {}

std::string UpdateSdfAssetCommand::getType() const
{
    return "UpdateSdfAsset";
}

int UpdateSdfAssetCommand::getVersion() const
{
    return 1;
}

std::string UpdateSdfAssetCommand::getDescription() const
{
    return "Command: UpdateSdfAsset\n"
           "Version: 1\n"
           "Purpose: Updates an existing SdfAsset's Formula and AABB bounds.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"UpdateSdfAsset\",\n"
           "  \"version\": 1,\n"
           "  \"assetGuid\": <string>,\n"
           "  \"sdf\": <Sdf JSON object>\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: None\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"UpdateSdfAsset\",\n"
           "  \"version\": 1,\n"
           "  \"assetGuid\": <string>\n"
           "}";
}

std::vector<CADacomb::GUID> UpdateSdfAssetCommand::getRequiredAssets() const
{
    return { assetGuid };
}

void UpdateSdfAssetCommand::setInput(const Polyglot::Node& node)
{
    assetGuid = node["assetGuid"].asString();
    newSdf    = CADacomb::Sdf::fromJson(node["sdf"].toJson());
}

nlohmann::json UpdateSdfAssetCommand::execute(Database& database)
{
    auto baseAsset = database.getProject().getAsset(assetGuid);
    if (!baseAsset)
    {
        return createErrorResponse("AssetNotFound", "Asset not found: " + assetGuid.toString());
    }

    auto sdfAsset = std::dynamic_pointer_cast<SdfAsset>(baseAsset);
    if (!sdfAsset)
    {
        return createErrorResponse("NotSdfAsset", "Asset is not an SdfAsset");
    }

    CADacomb::FormulaLuaEvaluator evaluator;
    std::string                   error;
    if (!evaluator.compile(newSdf.getFormula(), error))
    {
        return createErrorResponse("InvalidFormula", error.empty() ? "Failed to compile SDF formula" : error);
    }

    try
    {
        glm::dvec3         center = (newSdf.getAABB().getMin() + newSdf.getAABB().getMax()) * 0.5;
        std::vector<float> sample = evaluator.evaluate({ static_cast<float>(center.x), static_cast<float>(center.y), static_cast<float>(center.z) });
        if (sample.empty())
        {
            return createErrorResponse("InvalidFormula", "SDF formula produced no output value");
        }
    }
    catch (const std::exception& e)
    {
        return createErrorResponse("InvalidFormula", std::string("SDF formula evaluation failed: ") + e.what());
    }

    previousSdf = sdfAsset->getSdf();
    sdfAsset->setSdf(newSdf);
    executed = true;

    return createResponse({
      { "assetGuid", assetGuid.toString() }
    });
}

bool UpdateSdfAssetCommand::isUndoable() const
{
    return executed;
}

void UpdateSdfAssetCommand::undo(Database& database)
{
    auto sdfAsset = std::dynamic_pointer_cast<SdfAsset>(database.getProject().getAsset(assetGuid));
    if (sdfAsset)
    {
        sdfAsset->setSdf(previousSdf);
    }
}

void UpdateSdfAssetCommand::redo(Database& database)
{
    auto sdfAsset = std::dynamic_pointer_cast<SdfAsset>(database.getProject().getAsset(assetGuid));
    if (sdfAsset)
    {
        sdfAsset->setSdf(newSdf);
    }
}

size_t UpdateSdfAssetCommand::getUndoSize() const
{
    return sizeof(UpdateSdfAssetCommand) + previousSdf.getExpression().size() + newSdf.getExpression().size();
}

#ifdef ISTESTPROJECT
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("UpdateSdfAssetCommand/UpdateAndUndoRedo")
{
    Database database;
    auto     sdfAsset = std::make_shared<SdfAsset>();
    sdfAsset->setName("EditableSdf");
    database.getProject().addAsset(sdfAsset);

    CADacomb::Sdf updated("output[0] = length(float3(input[0], input[1], input[2])) - 0.8;", CADacomb::AABB3(glm::dvec3(-2.0), glm::dvec3(2.0)));

    UpdateSdfAssetCommand cmd;
    cmd.setInput({
      {      "type",                 "UpdateSdfAsset" },
      {   "version",                                1 },
      { "assetGuid",   sdfAsset->getGuid().toString() },
      {       "sdf", Polyglot::Node(updated.toJson()) }
    });
    nlohmann::json result = cmd.execute(database);
    CHECK(result.at("type").get<std::string>() == "UpdateSdfAsset");
    CHECK(sdfAsset->getAABB().getMax().x == Catch::Approx(2.0));

    cmd.undo(database);
    CHECK(sdfAsset->getAABB().getMax().x == Catch::Approx(1.0));

    cmd.redo(database);
    CHECK(sdfAsset->getAABB().getMax().x == Catch::Approx(2.0));
}
#endif
