#include "Search/CommandSearcher.h"

#include "Execution/Command.h"
#include "Execution/CommandFactory.h"

CommandSearcher::CommandSearcher(const CommandFactory& commandFactory)
  : commandFactory(commandFactory)
{
}

CommandSearcher::~CommandSearcher() {}

std::vector<SearchResult> CommandSearcher::search(const std::string& query)
{
    std::vector<SearchResult> results;
    if (query.empty())
    {
        return results;
    }

    auto commands = commandFactory.getAllCommands();
    for (const auto& command : commands)
    {
        if (!command)
        {
            continue;
        }

        const std::string& type        = command->getType();
        const std::string& description = command->getDescription();

        if (matcher.matchesAny(query, type, description))
        {
            results.emplace_back(type, "Command", description, nullptr);
        }
    }

    return results;
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("CommandSearcher/SearchMatching")
{
    CommandFactory  commandFactory;
    CommandSearcher searcher(commandFactory);

    auto results = searcher.search("NonExistentCommandXyz");
    REQUIRE(results.empty());

    auto addResults = searcher.search("AddNote");
    REQUIRE(!addResults.empty());
    REQUIRE(addResults[0].getTitle() == "AddNote");
    REQUIRE(addResults[0].getCategory() == "Command");
}
#endif
