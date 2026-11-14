#include "Commands/Search/SearchCommand.h"

#include "Execution/CommandFactory.h"
#include "Search/AssetSearcher.h"
#include "Search/CommandSearcher.h"
#include "Search/ConceptSearcher.h"
#include "Search/SearchService.h"

SearchCommand::SearchCommand()
  : query("")
  , category("All")
  , maxResults(20)
{
}

SearchCommand::~SearchCommand() {}

std::string SearchCommand::getType() const
{
    return "Search";
}

int SearchCommand::getVersion() const
{
    return 1;
}

std::string SearchCommand::getDescription() const
{
    return "Command: Search\n"
           "Version: 1\n"
           "Purpose: Executes a coarse, token- and fuzzy-based search across commands, concepts, assets, and notes.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"Search\",\n"
           "  \"version\": 1,\n"
           "  \"query\": \"<string>\",\n"
           "  \"category\": \"All\" | \"Command\" | \"Concept\" | \"Asset\" | \"Note\",\n"
           "  \"maxResults\": <int>\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: None\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"Search\",\n"
           "  \"version\": 1,\n"
           "  \"results\": [\n"
           "    {\n"
           "      \"title\": \"<string>\",\n"
           "      \"category\": \"<string>\",\n"
           "      \"summary\": \"<string>\"\n"
           "    }\n"
           "  ]\n"
           "}";
}

void SearchCommand::setInput(const Polyglot::Node& node)
{
    query      = node["query"].asString();
    category   = node["category"].asString();
    maxResults = node["maxResults"].asInt();
}

nlohmann::json SearchCommand::execute(Database& database)
{
    SearchService service;

    CommandFactory cmdFactory;
    if (category.empty() || category == "All" || category == "Command")
    {
        service.addSearcher(std::make_unique<CommandSearcher>(cmdFactory));
    }

    if (category.empty() || category == "All" || category == "Concept")
    {
        service.addSearcher(std::make_unique<ConceptSearcher>());
    }

    if (category.empty() || category == "All" || category == "Asset" || category == "Note")
    {
        service.addSearcher(std::make_unique<AssetSearcher>(database));
    }

    auto rawResults = service.search(query);

    nlohmann::json resultsJson = nlohmann::json::array();
    int            count       = 0;

    for (const auto& item : rawResults)
    {
        if (!category.empty() && category != "All" && item.getCategory() != category)
        {
            continue;
        }

        resultsJson.push_back({
          {    "title",       item.getTitle() },
          { "category",    item.getCategory() },
          {  "summary", item.getDescription() }
        });

        count++;
        if (maxResults > 0 && count >= maxResults)
        {
            break;
        }
    }

    return createResponse({
      { "results", resultsJson }
    });
}

#ifdef ISTESTPROJECT
#include "Database/Database.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("SearchCommand/CoarseSearch")
{
    Database       db;
    SearchCommand  cmd;
    Polyglot::Node input = {
        {       "type", "Search" },
        {    "version",        1 },
        {      "query",   "step" },
        {   "category",    "All" },
        { "maxResults",        5 }
    };
    cmd.setInput(input);
    auto response = cmd.execute(db);

    REQUIRE(response.at("type") == "Search");
    REQUIRE(response.at("results").is_array());
    REQUIRE(!response.at("results").empty());
    REQUIRE(response.at("results").size() <= 5);
}

TEST_CASE("SearchCommand/CategoryFilter")
{
    Database       db;
    SearchCommand  cmd;
    Polyglot::Node input = {
        {       "type",  "Search" },
        {    "version",         1 },
        {      "query",    "step" },
        {   "category", "Concept" },
        { "maxResults",        10 }
    };
    cmd.setInput(input);
    auto response = cmd.execute(db);

    REQUIRE(response.at("type") == "Search");
    for (const auto& r : response.at("results"))
    {
        REQUIRE(r.at("category") == "Concept");
    }
}
#endif
