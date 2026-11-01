#include "Search/CommandSearchProvider.h"

#include "Execution/Command.h"
#include "Execution/CommandFactory.h"
#include "Execution/GatedDatabase.h"
#include "Windows/Windows.h"

CommandSearchProvider::CommandSearchProvider(GatedDatabase& gatedDatabase, Windows& windows)
  : gatedDatabase(gatedDatabase)
  , windows(windows)
{
}

CommandSearchProvider::~CommandSearchProvider() {}

std::vector<SearchResult> CommandSearchProvider::search(const std::string& query)
{
    std::vector<SearchResult> results;
    if (query.empty())
    {
        return results;
    }

    auto commands = gatedDatabase.getCommandFactory().getAllCommands();
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
            results.emplace_back(type, "Command", description, [this]() { windows.open("CommandsInfoWindow"); });
        }
    }

    return results;
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("CommandSearchProvider/SearchMatching")
{
    GatedDatabase         gatedDatabase;
    Windows               windows;
    CommandSearchProvider provider(gatedDatabase, windows);

    auto results = provider.search("NonExistentCommandXyz");
    REQUIRE(results.empty());
}
#endif
