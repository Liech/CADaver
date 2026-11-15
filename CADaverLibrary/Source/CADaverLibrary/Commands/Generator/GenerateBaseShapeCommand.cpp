#include "Commands/Generator/GenerateBaseShapeCommand.h"

#include "CADacomb/Triangle/BaseShapeGenerator.h"
#include "CADacomb/Util/ProgressBar.h"
#include "Database/Asset/TriangulationAsset.h"
#include "Database/Database.h"

GenerateBaseShapeCommand::GenerateBaseShapeCommand() {}

GenerateBaseShapeCommand::~GenerateBaseShapeCommand() {}

std::string GenerateBaseShapeCommand::getType() const
{
    return "GenerateBaseShape";
}

int GenerateBaseShapeCommand::getVersion() const
{
    return 1;
}

std::string GenerateBaseShapeCommand::getDescription() const
{
    return "Command: GenerateBaseShape\n"
           "Version: 1\n"
           "Purpose: Generates a parametric triangulation shape asset using BaseShapeGenerator.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"GenerateBaseShape\",\n"
           "  \"version\": 1,\n"
           "  \"shape\": <string> (cube, box, pyramid, cylinder, cone, frustum, sphere, torus, capsule, prism, octahedron, cubeWithHole, lBracket, steppedBlock, plateWithHoles),\n"
           "  \"assetName\": <string> (optional),\n"
           "  \"width\": <number> (optional),\n"
           "  \"depth\": <number> (optional),\n"
           "  \"height\": <number> (optional),\n"
           "  \"length\": <number> (optional),\n"
           "  \"thickness\": <number> (optional),\n"
           "  \"radius\": <number> (optional),\n"
           "  \"bottomRadius\": <number> (optional),\n"
           "  \"topRadius\": <number> (optional),\n"
           "  \"majorRadius\": <number> (optional),\n"
           "  \"minorRadius\": <number> (optional),\n"
           "  \"holeRadius\": <number> (optional),\n"
           "  \"segments\": <int> (optional),\n"
           "  \"rings\": <int> (optional),\n"
           "  \"majorSegments\": <int> (optional),\n"
           "  \"minorSegments\": <int> (optional),\n"
           "  \"sides\": <int> (optional)\n"
           "}\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"GenerateBaseShape\",\n"
           "  \"version\": 1,\n"
           "  \"assetGuid\": <string>\n"
           "}";
}

void GenerateBaseShapeCommand::setInput(const Polyglot::Node& node)
{
    shape         = "cube";
    assetName     = "";
    width         = 1.0f;
    depth         = 1.0f;
    height        = 1.0f;
    length        = 2.0f;
    thickness     = 0.3f;
    radius        = 0.5f;
    bottomRadius  = 1.0f;
    topRadius     = 0.5f;
    majorRadius   = 1.0f;
    minorRadius   = 0.25f;
    holeRadius    = 0.3f;
    segments      = 32;
    rings         = 16;
    majorSegments = 32;
    minorSegments = 16;
    sides         = 6;

    if (node.contains("shape") && !node["shape"].isNull())
        shape = node["shape"].asString();
    if (node.contains("assetName") && !node["assetName"].isNull())
        assetName = node["assetName"].asString();
    if (node.contains("width") && !node["width"].isNull())
        width = static_cast<float>(node["width"].asDouble());
    if (node.contains("depth") && !node["depth"].isNull())
        depth = static_cast<float>(node["depth"].asDouble());
    if (node.contains("height") && !node["height"].isNull())
        height = static_cast<float>(node["height"].asDouble());
    if (node.contains("length") && !node["length"].isNull())
        length = static_cast<float>(node["length"].asDouble());
    if (node.contains("thickness") && !node["thickness"].isNull())
        thickness = static_cast<float>(node["thickness"].asDouble());
    if (node.contains("radius") && !node["radius"].isNull())
        radius = static_cast<float>(node["radius"].asDouble());
    if (node.contains("bottomRadius") && !node["bottomRadius"].isNull())
        bottomRadius = static_cast<float>(node["bottomRadius"].asDouble());
    if (node.contains("topRadius") && !node["topRadius"].isNull())
        topRadius = static_cast<float>(node["topRadius"].asDouble());
    if (node.contains("majorRadius") && !node["majorRadius"].isNull())
        majorRadius = static_cast<float>(node["majorRadius"].asDouble());
    if (node.contains("minorRadius") && !node["minorRadius"].isNull())
        minorRadius = static_cast<float>(node["minorRadius"].asDouble());
    if (node.contains("holeRadius") && !node["holeRadius"].isNull())
        holeRadius = static_cast<float>(node["holeRadius"].asDouble());
    if (node.contains("segments") && !node["segments"].isNull())
        segments = static_cast<size_t>(std::max(3, node["segments"].asInt()));
    if (node.contains("rings") && !node["rings"].isNull())
        rings = static_cast<size_t>(std::max(2, node["rings"].asInt()));
    if (node.contains("majorSegments") && !node["majorSegments"].isNull())
        majorSegments = static_cast<size_t>(std::max(3, node["majorSegments"].asInt()));
    if (node.contains("minorSegments") && !node["minorSegments"].isNull())
        minorSegments = static_cast<size_t>(std::max(3, node["minorSegments"].asInt()));
    if (node.contains("sides") && !node["sides"].isNull())
        sides = static_cast<size_t>(std::max(3, node["sides"].asInt()));
}

nlohmann::json GenerateBaseShapeCommand::execute(Database& database)
{
    CADacomb::ProgressBar bar(2, "Generating base shape");

    std::unique_ptr<CADacomb::Triangulation> tri = std::make_unique<CADacomb::Triangulation>();
    if (shape == "cube")
        *tri = CADacomb::BaseShapeGenerator::cube();
    else if (shape == "box")
        *tri = CADacomb::BaseShapeGenerator::box(width, depth, height);
    else if (shape == "pyramid")
        *tri = CADacomb::BaseShapeGenerator::pyramid();
    else if (shape == "cylinder")
        *tri = CADacomb::BaseShapeGenerator::cylinder(segments, height, radius);
    else if (shape == "cone")
        *tri = CADacomb::BaseShapeGenerator::cone(segments, radius, height);
    else if (shape == "frustum")
        *tri = CADacomb::BaseShapeGenerator::frustum(segments, bottomRadius, topRadius, height);
    else if (shape == "sphere")
        *tri = CADacomb::BaseShapeGenerator::sphere(segments, rings, radius);
    else if (shape == "torus")
        *tri = CADacomb::BaseShapeGenerator::torus(majorSegments, minorSegments, majorRadius, minorRadius);
    else if (shape == "capsule")
        *tri = CADacomb::BaseShapeGenerator::capsule(segments, rings, radius, height);
    else if (shape == "prism")
        *tri = CADacomb::BaseShapeGenerator::prism(sides, radius, height);
    else if (shape == "octahedron")
        *tri = CADacomb::BaseShapeGenerator::octahedron(radius);
    else if (shape == "cubeWithHole")
        *tri = CADacomb::BaseShapeGenerator::cubeWithHole(segments, holeRadius);
    else if (shape == "lBracket")
        *tri = CADacomb::BaseShapeGenerator::lBracket(length, width, height, thickness);
    else if (shape == "steppedBlock")
        *tri = CADacomb::BaseShapeGenerator::steppedBlock(length, width, height);
    else if (shape == "plateWithHoles")
        *tri = CADacomb::BaseShapeGenerator::plateWithHoles(segments, length, width, thickness, holeRadius);
    else
        return createErrorResponse("InvalidShape", "Unknown base shape: " + shape);

    bar.addProgress(1);

    auto triAsset = std::make_shared<TriangulationAsset>();
    triAsset->setName(assetName.empty() ? shape : assetName);
    triAsset->setTriangulation(std::move(tri));

    database.getProject().addAsset(triAsset);
    createdAsset = triAsset;
    bar.addProgress(1);

    return createResponse({
      { "assetGuid", triAsset->getGuid().toString() }
    });
}

bool GenerateBaseShapeCommand::isUndoable() const
{
    return createdAsset != nullptr;
}

void GenerateBaseShapeCommand::undo(Database& database)
{
    if (createdAsset)
    {
        database.getProject().removeAsset(createdAsset->getGuid());
    }
}

void GenerateBaseShapeCommand::redo(Database& database)
{
    if (createdAsset)
    {
        database.getProject().addAsset(createdAsset);
    }
}

size_t GenerateBaseShapeCommand::getUndoSize() const
{
    return sizeof(GenerateBaseShapeCommand) + (createdAsset ? createdAsset->getSizeInBytes() : 0);
}

#ifdef ISTESTPROJECT
#include "CADacomb/Triangle/HalfEdge/HalfEdgeHealth.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("GenerateBaseShapeCommand/AllShapesAndUndoRedo")
{
    std::vector<std::string> shapes = {
        "cube",
        "box",
        "pyramid",
        "cylinder",
        "cone",
        "frustum",
        "sphere",
        "torus",
        "capsule",
        "prism",
        "octahedron",
        "cubeWithHole",
        "lBracket",
        "steppedBlock",
        "plateWithHoles"
    };

    Database db;
    for (size_t i = 0; i < shapes.size(); i++)
    {
        GenerateBaseShapeCommand cmd;
        cmd.setInput({
          {    "type", "GenerateBaseShape" },
          { "version",                   1 },
          {   "shape",           shapes[i] }
        });
        nlohmann::json result = cmd.execute(db);
        REQUIRE_FALSE(result.contains("error"));
        REQUIRE(result.contains("assetGuid"));
        std::string guidStr = result["assetGuid"].get<std::string>();
        auto        asset   = std::dynamic_pointer_cast<TriangulationAsset>(db.getProject().getAsset(CADacomb::GUID(guidStr)));
        REQUIRE(asset != nullptr);
        REQUIRE(CADacomb::HalfEdgeHealth::isHealthy(asset->getTriangulation()));
        REQUIRE(cmd.isUndoable());

        cmd.undo(db);
        REQUIRE(db.getProject().getAsset(CADacomb::GUID(guidStr)) == nullptr);

        cmd.redo(db);
        REQUIRE(db.getProject().getAsset(CADacomb::GUID(guidStr)) != nullptr);
    }
}

TEST_CASE("GenerateBaseShapeCommand/InvalidShape")
{
    Database                 db;
    GenerateBaseShapeCommand cmd;
    cmd.setInput({
      {    "type", "GenerateBaseShape" },
      { "version",                   1 },
      {   "shape",       "nonExistent" }
    });
    nlohmann::json result = cmd.execute(db);
    REQUIRE(result.at("type") == "Error");
    REQUIRE(result.at("errorType") == "InvalidShape");
    REQUIRE(!cmd.isUndoable());
}
#endif
