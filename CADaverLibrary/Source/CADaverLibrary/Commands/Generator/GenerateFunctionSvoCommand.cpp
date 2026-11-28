#include "Commands/Generator/GenerateFunctionSvoCommand.h"

#include "CADacomb/Formula/FormulaLuaEmitter.h"
#include "Database/Asset/VoxelAsset.h"
#include "Database/Database.h"
#include "Utility/LuaSVOField.h"

GenerateFunctionSvoCommand::GenerateFunctionSvoCommand() {}

GenerateFunctionSvoCommand::~GenerateFunctionSvoCommand() {}

std::string GenerateFunctionSvoCommand::getType() const
{
    return "GenerateFunctionSvo";
}

int GenerateFunctionSvoCommand::getVersion() const
{
    return 1;
}

std::string GenerateFunctionSvoCommand::getDescription() const
{
    return "Command: GenerateFunctionSvo\n"
           "Version: 1\n"
           "Purpose: Generates a volumetric SVO asset by evaluating a Slang-dialect Formula mapping input[0..2] in [-1, 1]^3 to output[0] (< 0 inside).\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"GenerateFunctionSvo\",\n"
           "  \"version\": 1,\n"
           "  \"assetName\": <string>,\n"
           "  \"dimension\": <uint>,\n"
           "  \"formula\": <Formula JSON object>\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: None\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"GenerateFunctionSvo\",\n"
           "  \"version\": 1,\n"
           "  \"assetGuid\": <string>\n"
           "}";
}

void GenerateFunctionSvoCommand::setInput(const Polyglot::Node& node)
{
    dimension = static_cast<uint32_t>(node["dimension"].asInt());
    assetName = node["assetName"].asString();
    formula   = CADacomb::Formula::fromJson(node["formula"].toJson());
}

nlohmann::json GenerateFunctionSvoCommand::execute(Database& database)
{
    std::string script = CADacomb::FormulaLuaEmitter::emit(formula);
    script += "\nfunction sdf(x, y, z)\n";
    script += "    return " + formula.getName() + "({ x, y, z })[1]\n";
    script += "end\n";

    std::string error;
    auto        svo = LuaSVOField::generateSVO(dimension, script, error);

    if (!svo)
    {
        return createErrorResponse("GenerationFailed", error.empty() ? "Failed to generate function SVO" : error);
    }

    auto voxelAsset = std::make_shared<VoxelAsset>();
    voxelAsset->setName(assetName);
    voxelAsset->setSVO(std::move(svo));
    voxelAsset->setAABB(CADacomb::AABB3(glm::dvec3(0.0), glm::dvec3(static_cast<double>(dimension))));

    database.getProject().addAsset(voxelAsset);
    createdAsset = voxelAsset;

    return createResponse({
      { "assetGuid", voxelAsset->getGuid().toString() }
    });
}

bool GenerateFunctionSvoCommand::isUndoable() const
{
    return createdAsset != nullptr;
}

void GenerateFunctionSvoCommand::undo(Database& database)
{
    if (createdAsset)
    {
        database.getProject().removeAsset(createdAsset->getGuid());
    }
}

void GenerateFunctionSvoCommand::redo(Database& database)
{
    if (createdAsset)
    {
        database.getProject().addAsset(createdAsset);
    }
}

size_t GenerateFunctionSvoCommand::getUndoSize() const
{
    return sizeof(GenerateFunctionSvoCommand) + (createdAsset ? createdAsset->getSizeInBytes() : 0);
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("GenerateFunctionSvoCommand/TypeAndVersion")
{
    GenerateFunctionSvoCommand cmd;
    CHECK(cmd.getType() == "GenerateFunctionSvo");
    CHECK(cmd.getVersion() == 1);
    CHECK_FALSE(cmd.getDescription().empty());
}

TEST_CASE("GenerateFunctionSvoCommand/ExecuteFormulaAndUndoRedo")
{
    CADacomb::Formula formula;
    formula.setName("sphereSdf");
    formula.setInputCount(3);
    formula.setOutputCount(1);
    formula.setExpression("float3 p = float3(input[0], input[1], input[2]);\noutput[0] = length(p) - 0.5;");

    Database                   database;
    GenerateFunctionSvoCommand cmd;
    cmd.setInput({
      {      "type",            "GenerateFunctionSvo" },
      {   "version",                                1 },
      { "assetName",                  "FormulaSphere" },
      { "dimension",                               16 },
      {   "formula", Polyglot::Node(formula.toJson()) }
    });
    nlohmann::json result = cmd.execute(database);

    CHECK(result.at("type").get<std::string>() == "GenerateFunctionSvo");
    std::string guidStr = result.at("assetGuid").get<std::string>();
    CHECK_FALSE(guidStr.empty());

    CADacomb::GUID guid(guidStr);
    REQUIRE(database.getProject().hasAsset(guid));
    CHECK(cmd.isUndoable());

    cmd.undo(database);
    CHECK_FALSE(database.getProject().hasAsset(guid));

    cmd.redo(database);
    CHECK(database.getProject().hasAsset(guid));
}
#endif
