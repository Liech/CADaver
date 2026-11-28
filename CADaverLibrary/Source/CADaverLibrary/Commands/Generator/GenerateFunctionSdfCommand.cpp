#include "Commands/Generator/GenerateFunctionSdfCommand.h"

#include "CADacomb/Formula/FormulaLuaEvaluator.h"
#include "Database/Asset/SdfAsset.h"
#include "Database/Database.h"

GenerateFunctionSdfCommand::GenerateFunctionSdfCommand() {}

GenerateFunctionSdfCommand::~GenerateFunctionSdfCommand() {}

std::string GenerateFunctionSdfCommand::getType() const
{
    return "GenerateFunctionSdf";
}

int GenerateFunctionSdfCommand::getVersion() const
{
    return 1;
}

std::string GenerateFunctionSdfCommand::getDescription() const
{
    return "Command: GenerateFunctionSdf\n"
           "Version: 1\n"
           "Purpose: Generates an SdfAsset from a Slang-dialect Formula mapping input[0..2] to signed distance output[0] (< 0 inside).\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"GenerateFunctionSdf\",\n"
           "  \"version\": 1,\n"
           "  \"assetName\": <string>,\n"
           "  \"formula\": <Formula JSON object>\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: None\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"GenerateFunctionSdf\",\n"
           "  \"version\": 1,\n"
           "  \"assetGuid\": <string>\n"
           "}";
}

void GenerateFunctionSdfCommand::setInput(const Polyglot::Node& node)
{
    assetName = node["assetName"].asString();
    formula   = CADacomb::Formula::fromJson(node["formula"].toJson());
}

nlohmann::json GenerateFunctionSdfCommand::execute(Database& database)
{
    CADacomb::FormulaLuaEvaluator evaluator;
    std::string                   error;
    if (!evaluator.compile(formula, error))
    {
        return createErrorResponse("GenerationFailed", error.empty() ? "Failed to compile SDF formula" : error);
    }

    try
    {
        glm::dvec3         center = (bounds.getMin() + bounds.getMax()) * 0.5;
        std::vector<float> sample = evaluator.evaluate({ static_cast<float>(center.x), static_cast<float>(center.y), static_cast<float>(center.z) });
        if (sample.empty())
        {
            return createErrorResponse("GenerationFailed", "SDF formula produced no output value");
        }
    }
    catch (const std::exception& e)
    {
        return createErrorResponse("GenerationFailed", std::string("SDF formula evaluation failed: ") + e.what());
    }

    auto sdfAsset = std::make_shared<SdfAsset>();
    sdfAsset->setName(assetName);
    sdfAsset->setFormula(formula);
    sdfAsset->setAABB(bounds);

    database.getProject().addAsset(sdfAsset);
    createdAsset = sdfAsset;

    return createResponse({
      { "assetGuid", sdfAsset->getGuid().toString() }
    });
}

bool GenerateFunctionSdfCommand::isUndoable() const
{
    return createdAsset != nullptr;
}

void GenerateFunctionSdfCommand::undo(Database& database)
{
    if (createdAsset)
    {
        database.getProject().removeAsset(createdAsset->getGuid());
    }
}

void GenerateFunctionSdfCommand::redo(Database& database)
{
    if (createdAsset)
    {
        database.getProject().addAsset(createdAsset);
    }
}

size_t GenerateFunctionSdfCommand::getUndoSize() const
{
    return sizeof(GenerateFunctionSdfCommand) + (createdAsset ? createdAsset->getSizeInBytes() : 0);
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("GenerateFunctionSdfCommand/TypeAndVersion")
{
    GenerateFunctionSdfCommand cmd;
    CHECK(cmd.getType() == "GenerateFunctionSdf");
    CHECK(cmd.getVersion() == 1);
    CHECK_FALSE(cmd.getDescription().empty());
}

TEST_CASE("GenerateFunctionSdfCommand/ExecuteFormulaAndUndoRedo")
{
    CADacomb::Formula formula;
    formula.setName("sphereSdf");
    formula.setInputCount(3);
    formula.setOutputCount(1);
    formula.setExpression("float3 p = float3(input[0], input[1], input[2]);\noutput[0] = length(p) - 0.5;");

    Database                   database;
    GenerateFunctionSdfCommand cmd;
    cmd.setInput({
      {      "type",            "GenerateFunctionSdf" },
      {   "version",                                1 },
      { "assetName",                     "FormulaSdf" },
      {   "formula", Polyglot::Node(formula.toJson()) }
    });
    nlohmann::json result = cmd.execute(database);

    CHECK(result.at("type").get<std::string>() == "GenerateFunctionSdf");
    std::string guidStr = result.at("assetGuid").get<std::string>();
    CHECK_FALSE(guidStr.empty());

    CADacomb::GUID guid(guidStr);
    REQUIRE(database.getProject().hasAsset(guid));
    CHECK(database.getProject().getAsset(guid)->getType() == "Sdf");
    CHECK(cmd.isUndoable());

    cmd.undo(database);
    CHECK_FALSE(database.getProject().hasAsset(guid));

    cmd.redo(database);
    CHECK(database.getProject().hasAsset(guid));
}
#endif
