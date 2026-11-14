#include "Commands/Search/GetDocumentationCommand.h"

#include "Concept/Concept.h"
#include "Concept/ConceptFactory.h"
#include "Database/Asset.h"
#include "Database/Asset/CadAsset.h"
#include "Database/Asset/ProgressBarAsset.h"
#include "Database/Asset/TriangulationAsset.h"
#include "Database/Asset/VoxelAsset.h"
#include "Database/Component/TextNoteComponent.h"
#include "Database/Database.h"
#include "Database/Project.h"
#include "Execution/Command.h"
#include "Execution/CommandFactory.h"

GetDocumentationCommand::GetDocumentationCommand()
  : category("")
  , name("")
{
}

GetDocumentationCommand::~GetDocumentationCommand() {}

std::string GetDocumentationCommand::getType() const
{
    return "GetDocumentation";
}

int GetDocumentationCommand::getVersion() const
{
    return 1;
}

std::string GetDocumentationCommand::getDescription() const
{
    return "Command: GetDocumentation\n"
           "Version: 1\n"
           "Purpose: Retrieves full documentation, JSON input/output schema, or complete text for a command, concept, asset, or note.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"GetDocumentation\",\n"
           "  \"version\": 1,\n"
           "  \"category\": \"Command\" | \"Concept\" | \"Asset\" | \"Note\",\n"
           "  \"name\": \"<string>\"\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: None\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"GetDocumentation\",\n"
           "  \"version\": 1,\n"
           "  \"category\": \"<string>\",\n"
           "  \"name\": \"<string>\",\n"
           "  \"content\": \"<string>\"\n"
           "}";
}

void GetDocumentationCommand::setInput(const Polyglot::Node& node)
{
    category = node["category"].asString();
    name     = node["name"].asString();
}

nlohmann::json GetDocumentationCommand::execute(Database& database)
{

    if (category == "Command")
    {
        CommandFactory factory;
        auto           cmd = factory.create(name);
        if (cmd)
        {
            return createResponse({
              { "category",              category },
              {     "name",                  name },
              {  "content", cmd->getDescription() }
            });
        }
        return createErrorResponse("NotFound", "Command not found: " + name);
    }

    if (category == "Concept")
    {
        ConceptFactory factory;
        auto           item = factory.create(name);
        if (item)
        {
            return createResponse({
              { "category",               category },
              {     "name",                   name },
              {  "content", item->getDescription() }
            });
        }
        return createErrorResponse("NotFound", "Concept not found: " + name);
    }

    if (category == "Asset")
    {
        const auto& assets = database.getProject().getAssets();
        for (const auto& asset : assets)
        {
            if (asset && (asset->getName() == name || asset->getGuid().toString() == name))
            {
                std::string content =
                  "Asset Name: " + asset->getName() + "\n" + "Type: " + asset->getType() + "\n" + "GUID: " + asset->getGuid().toString() + "\n" + "Revision: " + std::to_string(asset->getRevision());

                auto voxel = std::dynamic_pointer_cast<const VoxelAsset>(asset);
                if (voxel)
                {
                    uint32_t dim = voxel->getSVO().getDimension();
                    content += "\nResolution: " + std::to_string(dim) + "x" + std::to_string(dim) + "x" + std::to_string(dim);
                }

                auto tri = std::dynamic_pointer_cast<const TriangulationAsset>(asset);
                if (tri)
                {
                    const auto& triangulation = tri->getTriangulation();
                    size_t      triCount      = triangulation.indices.empty() ? (triangulation.vertices.size() / 3) : (triangulation.indices.size() / 3);
                    content += "\nTriangles: " + std::to_string(triCount);
                }

                auto cad = std::dynamic_pointer_cast<const CadAsset>(asset);
                if (cad)
                {
                    const auto& triangulation = cad->getTriangulation();
                    size_t      triCount      = triangulation.indices.empty() ? (triangulation.vertices.size() / 3) : (triangulation.indices.size() / 3);
                    content += "\nTriangles: " + std::to_string(triCount);
                }

                auto notes = asset->getAttachments<TextNoteComponent>();
                if (!notes.empty())
                {
                    content += "\nAttached Notes (" + std::to_string(notes.size()) + "):";
                    for (const auto& note : notes)
                    {
                        if (note)
                        {
                            content += "\n - [" + note->getName() + "] (GUID: " + note->getGuid().toString() + "):\n" + note->getText();
                        }
                    }
                }

                return createResponse({
                  { "category", category },
                  {     "name",     name },
                  {  "content",  content }
                });
            }
        }

        return createErrorResponse("NotFound", "Asset not found: " + name);
    }

    if (category == "Note")
    {
        const auto& assets = database.getProject().getAssets();
        for (const auto& asset : assets)
        {
            if (!asset)
            {
                continue;
            }
            auto notes = asset->getAttachments<TextNoteComponent>();
            for (const auto& note : notes)
            {
                if (note && (note->getName() == name || note->getGuid().toString() == name))
                {
                    std::string content = "Parent Asset: " + asset->getName() + " (" + asset->getType() + ")\n" + "Note Name: " + note->getName() + "\n" + "Note GUID: " + note->getGuid().toString() +
                                          "\n\n" + "Content:\n" + note->getText();
                    return createResponse({
                      { "category", category },
                      {     "name",     name },
                      {  "content",  content }
                    });
                }
            }
        }

        return createErrorResponse("NotFound", "Note not found: " + name);
    }

    return createErrorResponse("InvalidCategory", "Invalid documentation category: " + category);
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("GetDocumentationCommand/CommandAndConcept")
{
    Database                db;
    GetDocumentationCommand cmd;

    nlohmann::json cmdInput = {
        {     "type", "GetDocumentation" },
        {  "version",                  1 },
        { "category",          "Command" },
        {     "name",          "AddNote" }
    };
    cmd.setInput(cmdInput);
    auto cmdRes = cmd.execute(db);

    REQUIRE(cmdRes.at("type") == "GetDocumentation");
    REQUIRE(!cmdRes.at("content").get<std::string>().empty());

    nlohmann::json conceptInput = {
        {     "type", "GetDocumentation" },
        {  "version",                  1 },
        { "category",          "Concept" },
        {     "name",             "STEP" }
    };
    cmd.setInput(conceptInput);
    auto conceptRes = cmd.execute(db);

    REQUIRE(conceptRes.at("type") == "GetDocumentation");
    REQUIRE(!conceptRes.at("content").get<std::string>().empty());

    nlohmann::json notFoundInput = {
        {     "type",  "GetDocumentation" },
        {  "version",                   1 },
        { "category",           "Command" },
        {     "name", "UnknownCommandXYZ" }
    };
    cmd.setInput(notFoundInput);
    auto notFoundRes = cmd.execute(db);
    REQUIRE(notFoundRes.at("type") == "Error");
    REQUIRE(notFoundRes.at("command") == "GetDocumentation");
    REQUIRE(notFoundRes.at("errorType") == "NotFound");
}
#endif
