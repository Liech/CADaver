#include "Commands/Conversion/ConvertStlToCadCommand.h"

#include "CADacomb/CAD/Conversion/NaiveMeshToCadConverter.h"
#include "CADacomb/CAD/Interface/Shape.h"
#include "CADacomb/CAD/Sm/KernelSm.h"
#include "CADacomb/Triangle/Triangulation.h"
#include "CADacomb/Util/ProgressBar.h"
#include "Database/Asset/CadAsset.h"
#include "Database/Asset/TriangulationAsset.h"
#include "Database/Database.h"

ConvertStlToCadCommand::ConvertStlToCadCommand() {}

ConvertStlToCadCommand::~ConvertStlToCadCommand() {}

std::string ConvertStlToCadCommand::getType() const
{
    return "ConvertStlToCad";
}

int ConvertStlToCadCommand::getVersion() const
{
    return 1;
}

std::string ConvertStlToCadCommand::getDescription() const
{
    return "Command: ConvertStlToCad\n"
           "Version: 1\n"
           "Purpose: Converts a TriangulationAsset naively (one planar CAD face per triangle) into a CadAsset.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"ConvertStlToCad\",\n"
           "  \"version\": 1,\n"
           "  \"assetGuid\": <string>,\n"
           "  \"stitchTolerance\": <number> (optional, default 1e-4),\n"
           "  \"tryMakeSolid\": <bool> (optional, default true)\n"
           "}\n\n"
           "Output (JSON):\n"
           "{\n"
           "  \"type\": \"ConvertStlToCad\",\n"
           "  \"version\": 1,\n"
           "  \"assetGuid\": <string>,\n"
           "  \"createdAssetGuid\": <string>\n"
           "}";
}

std::vector<CADacomb::GUID> ConvertStlToCadCommand::getRequiredAssets() const
{
    return { assetGuid };
}

void ConvertStlToCadCommand::setInput(const Polyglot::Node& node)
{
    assetGuid       = CADacomb::GUID(node["assetGuid"].asString());
    stitchTolerance = 1e-4;
    tryMakeSolid    = true;

    if (node.contains("stitchTolerance") && !node["stitchTolerance"].isNull())
    {
        stitchTolerance = node["stitchTolerance"].asDouble();
    }
    if (node.contains("tryMakeSolid") && !node["tryMakeSolid"].isNull())
    {
        tryMakeSolid = node["tryMakeSolid"].asBool();
    }
}

nlohmann::json ConvertStlToCadCommand::execute(Database& database)
{
    CADacomb::ProgressBar bar(2, "Converting STL to CAD");

    Project&               project = database.getProject();
    std::shared_ptr<Asset> asset   = project.getAsset(assetGuid);
    if (!asset)
    {
        return createErrorResponse("AssetNotFound", "Asset not found: " + assetGuid.toString());
    }

    std::shared_ptr<TriangulationAsset> triAsset = std::dynamic_pointer_cast<TriangulationAsset>(asset);
    if (!triAsset)
    {
        return createErrorResponse("IncompatibleAsset", "Asset is not a TriangulationAsset: " + assetGuid.toString());
    }

    if (bar.isCanceled())
    {
        return createErrorResponse("Canceled", "Converting STL to CAD was canceled");
    }

    CADacomb::CAD::KernelSm                kernel;
    CADacomb::CAD::NaiveMeshToCadConverter converter;
    std::unique_ptr<CADacomb::CAD::Shape>  shape = converter.convert(kernel, triAsset->getTriangulation(), stitchTolerance, tryMakeSolid);
    bar.addProgress(1);

    createdAsset = std::make_shared<CadAsset>();
    createdAsset->setName(triAsset->getName() + "_CAD");
    createdAsset->setCadShape(std::shared_ptr<CADacomb::CAD::Shape>(std::move(shape)));
    project.addAsset(createdAsset);
    bar.addProgress(1);

    return createResponse({
      {        "assetGuid",              assetGuid.toString() },
      { "createdAssetGuid", createdAsset->getGuid().toString() }
    });
}

bool ConvertStlToCadCommand::isUndoable() const
{
    return true;
}

void ConvertStlToCadCommand::undo(Database& database)
{
    if (createdAsset)
    {
        database.getProject().removeAsset(createdAsset->getGuid());
    }
}

void ConvertStlToCadCommand::redo(Database& database)
{
    if (createdAsset)
    {
        database.getProject().addAsset(createdAsset);
    }
}

size_t ConvertStlToCadCommand::getUndoSize() const
{
    return sizeof(*this);
}

#ifdef ISTESTPROJECT
#include "CADacomb/Triangle/BaseShapeGenerator.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("ConvertStlToCadCommand/CubeToCad")
{
    Database db;
    auto     tri      = std::make_unique<CADacomb::Triangulation>(CADacomb::BaseShapeGenerator::cube());
    auto     triAsset = std::make_shared<TriangulationAsset>();
    triAsset->setName("Cube");
    triAsset->setTriangulation(std::move(tri));
    db.getProject().addAsset(triAsset);

    ConvertStlToCadCommand cmd;
    cmd.setInput(Polyglot::Node(nlohmann::json{
      {            "type",                  "ConvertStlToCad" },
      {         "version",                                  1 },
      {       "assetGuid", triAsset->getGuid().toString() },
      { "stitchTolerance",                               1e-4 },
      {    "tryMakeSolid",                               true }
    }));

    nlohmann::json result = cmd.execute(db);

    REQUIRE(result.at("type") == "ConvertStlToCad");
    REQUIRE(!result.contains("error"));

    CADacomb::GUID createdGuid(result.at("createdAssetGuid").get<std::string>());
    auto           created = std::dynamic_pointer_cast<CadAsset>(db.getProject().getAsset(createdGuid));
    REQUIRE(created != nullptr);
    REQUIRE(created->getTriangulation().indices.size() >= 36);

    cmd.undo(db);
    REQUIRE(db.getProject().getAsset(createdGuid) == nullptr);

    cmd.redo(db);
    REQUIRE(db.getProject().getAsset(createdGuid) != nullptr);
}
#endif
