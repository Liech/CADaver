#include "Commands/Transform/TransformAssetCommand.h"

#include "Database/Asset.h"
#include "Database/Database.h"
#include "Database/Project.h"
#include "Utility/MatrixJson.h"

TransformAssetCommand::TransformAssetCommand() {}

TransformAssetCommand::~TransformAssetCommand() {}

std::string TransformAssetCommand::getType() const
{
    return "TransformAsset";
}

int TransformAssetCommand::getVersion() const
{
    return 1;
}

std::string TransformAssetCommand::getDescription() const
{
    return "Command: TransformAsset\n"
           "Version: 1\n"
           "Purpose: Sets the absolute 4x4 transformation matrix of an asset.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"TransformAsset\",\n"
           "  \"version\": 1,\n"
           "  \"assetGuid\": <string>,\n"
           "  \"transformation\": [<double> x16, column-major]\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: None\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"TransformAsset\",\n"
           "  \"version\": 1\n"
           "}";
}

std::vector<CADacomb::GUID> TransformAssetCommand::getRequiredAssets() const
{
    return { assetGuid };
}

void TransformAssetCommand::setInput(const Polyglot::Node& node)
{
    assetGuid      = CADacomb::GUID(node["assetGuid"].asString());
    transformation = MatrixJson::fromJson(node["transformation"].toJson());
    if (node.contains("mergeGuid"))
    {
        setMergeGuid(CADacomb::GUID(node["mergeGuid"].asString()));
    }
}

nlohmann::json TransformAssetCommand::execute(Database& database)
{
    auto asset = database.getProject().getAsset(assetGuid);
    if (!asset)
    {
        return createErrorResponse("AssetNotFound", "Asset not found: " + assetGuid.toString());
    }

    previousTransformation = asset->getTransformation();
    asset->setTransformation(transformation);
    return createResponse();
}

bool TransformAssetCommand::isUndoable() const
{
    return true;
}

void TransformAssetCommand::undo(Database& database)
{
    database.getProject().getAsset(assetGuid)->setTransformation(previousTransformation);
}

void TransformAssetCommand::redo(Database& database)
{
    database.getProject().getAsset(assetGuid)->setTransformation(transformation);
}

size_t TransformAssetCommand::getUndoSize() const
{
    return sizeof(TransformAssetCommand);
}

bool TransformAssetCommand::isMergeable() const
{
    return true;
}

bool TransformAssetCommand::merge(const Command& other)
{
    const auto* otherTransform = dynamic_cast<const TransformAssetCommand*>(&other);
    if (!otherTransform || otherTransform->assetGuid != assetGuid)
    {
        return false;
    }

    transformation = otherTransform->transformation;
    return true;
}

#ifdef ISTESTPROJECT
#include "Database/Asset/TriangulationAsset.h"

#include <catch2/catch_test_macros.hpp>
#include <glm/gtc/matrix_transform.hpp>

TEST_CASE("TransformAssetCommand/SetsAndRestoresOnUndoRedo")
{
    Database database;
    auto     asset = std::make_shared<TriangulationAsset>();
    database.getProject().addAsset(asset);

    glm::dmat4 target = glm::translate(glm::dmat4(1.0), glm::dvec3(1.0, 2.0, 3.0));

    TransformAssetCommand cmd;
    cmd.setInput(Polyglot::Node({
      {      "assetGuid", asset->getGuid().toString() },
      { "transformation",  MatrixJson::toJson(target) }
    }));
    size_t         revisionBefore = asset->getRevision();
    nlohmann::json res            = cmd.execute(database);
    CHECK(res.at("type").get<std::string>() == "TransformAsset");
    CHECK(asset->getTransformation() == target);
    CHECK(asset->getRevision() > revisionBefore);

    cmd.undo(database);
    CHECK(asset->getTransformation() == glm::dmat4(1.0));

    cmd.redo(database);
    CHECK(asset->getTransformation() == target);
}

TEST_CASE("TransformAssetCommand/UnknownAssetReturnsError")
{
    Database              database;
    TransformAssetCommand cmd;
    cmd.setInput(Polyglot::Node({
      {      "assetGuid", CADacomb::GUID::random().toString() },
      { "transformation", MatrixJson::toJson(glm::dmat4(1.0)) }
    }));
    nlohmann::json res = cmd.execute(database);
    CHECK(res.at("type").get<std::string>() == "Error");
    CHECK(res.at("errorType").get<std::string>() == "AssetNotFound");
}

TEST_CASE("TransformAssetCommand/MergePreservesOriginalPreviousAndUpdatesTarget")
{
    Database database;
    auto     asset = std::make_shared<TriangulationAsset>();
    database.getProject().addAsset(asset);

    glm::dmat4 t1 = glm::translate(glm::dmat4(1.0), glm::dvec3(1.0, 0.0, 0.0));
    glm::dmat4 t2 = glm::translate(glm::dmat4(1.0), glm::dvec3(4.0, 5.0, 6.0));

    TransformAssetCommand cmd1;
    cmd1.setInput(Polyglot::Node({
      {      "assetGuid", asset->getGuid().toString() },
      { "transformation",      MatrixJson::toJson(t1) }
    }));
    cmd1.execute(database);

    TransformAssetCommand cmd2;
    cmd2.setInput(Polyglot::Node({
      {      "assetGuid", asset->getGuid().toString() },
      { "transformation",      MatrixJson::toJson(t2) }
    }));
    cmd2.execute(database);

    REQUIRE(cmd1.isMergeable());
    REQUIRE(cmd1.merge(cmd2));

    cmd1.undo(database);
    CHECK(asset->getTransformation() == glm::dmat4(1.0));

    cmd1.redo(database);
    CHECK(asset->getTransformation() == t2);
}
#endif
