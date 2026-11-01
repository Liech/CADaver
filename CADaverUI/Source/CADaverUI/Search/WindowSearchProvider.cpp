#include "Search/WindowSearchProvider.h"

#include "Windows/Window.h"
#include "Windows/WindowFactory.h"
#include "Windows/Windows.h"

WindowSearchProvider::WindowSearchProvider(Windows& windows)
  : windows(windows)
{
}

WindowSearchProvider::~WindowSearchProvider() {}

std::vector<SearchResult> WindowSearchProvider::search(const std::string& query)
{
    std::vector<SearchResult> results;
    if (query.empty())
    {
        return results;
    }

    WindowFactory factory;
    auto          allWindows = factory.getAllWindows();

    for (const auto& win : allWindows)
    {
        if (!win)
        {
            continue;
        }

        std::string type        = win->getType();
        std::string description = win->getDescription();

        if (matcher.matchesAny(query, type, description))
        {
            results.emplace_back(type, "Window", description, [this, type]() { windows.open(type); });
        }
    }

    return results;
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("WindowSearchProvider/SearchMatching")
{
    Windows              windows;
    WindowSearchProvider provider(windows);

    auto results = provider.search("About");
    REQUIRE(!results.empty());
    REQUIRE(results[0].getTitle() == "About");
}
#endif
