#include "Search/SearchService.h"

#include <algorithm>

SearchService::SearchService() {}

SearchService::~SearchService() {}

void SearchService::addProvider(std::unique_ptr<SearchProvider> provider)
{
    if (provider)
    {
        providers.push_back(std::move(provider));
    }
}

void SearchService::clearProviders()
{
    providers.clear();
}

std::vector<SearchResult> SearchService::search(const std::string& query) const
{
    std::vector<SearchResult> combinedResults;
    if (query.empty())
    {
        return combinedResults;
    }

    for (const auto& provider : providers)
    {
        if (provider)
        {
            auto results = provider->search(query);
            combinedResults.insert(combinedResults.end(), std::make_move_iterator(results.begin()), std::make_move_iterator(results.end()));
        }
    }

    std::stable_sort(
      combinedResults.begin(), combinedResults.end(), [this](const SearchResult& a, const SearchResult& b) { return getCategoryRank(a.getCategory()) < getCategoryRank(b.getCategory()); });

    return combinedResults;
}

int SearchService::getCategoryRank(const std::string& category) const
{
    if (category == "Asset")
    {
        return 0;
    }
    if (category == "Note")
    {
        return 1;
    }
    if (category == "Window")
    {
        return 2;
    }
    if (category == "Concept")
    {
        return 3;
    }
    if (category == "Command")
    {
        return 100;
    }
    return 50;
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

class MockSearchProvider : public SearchProvider
{
  public:
    MockSearchProvider() {}
    virtual ~MockSearchProvider() {}

    std::vector<SearchResult> search(const std::string& query) override
    {
        std::vector<SearchResult> res;
        if (query == "test")
        {
            res.emplace_back("MockItem", "Mock", "Mock description", nullptr);
        }
        return res;
    }
};

TEST_CASE("SearchService/Aggregation")
{
    SearchService service;
    service.addProvider(std::make_unique<MockSearchProvider>());

    auto emptyResults = service.search("");
    REQUIRE(emptyResults.empty());

    auto results = service.search("test");
    REQUIRE(results.size() == 1);
    REQUIRE(results[0].getTitle() == "MockItem");
}
#endif
