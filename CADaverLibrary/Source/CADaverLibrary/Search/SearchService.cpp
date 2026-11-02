#include "Search/SearchService.h"

#include <algorithm>

SearchService::SearchService() {}

SearchService::~SearchService() {}

void SearchService::addSearcher(std::unique_ptr<Searcher> searcher)
{
    if (searcher)
    {
        searchers.push_back(std::move(searcher));
    }
}

void SearchService::clearSearchers()
{
    searchers.clear();
}

std::vector<SearchResult> SearchService::search(const std::string& query) const
{
    std::vector<SearchResult> combinedResults;
    if (query.empty())
    {
        return combinedResults;
    }

    for (const auto& searcher : searchers)
    {
        if (searcher)
        {
            auto results = searcher->search(query);
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
    if (category == "UI" || category == "Menu")
    {
        return 2;
    }
    if (category == "Preferences")
    {
        return 3;
    }
    if (category == "Window")
    {
        return 4;
    }
    if (category == "Concept")
    {
        return 5;
    }
    if (category == "Command")
    {
        return 100;
    }
    return 50;
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

class MockSearcher : public Searcher
{
  public:
    MockSearcher() {}
    virtual ~MockSearcher() {}

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
    service.addSearcher(std::make_unique<MockSearcher>());

    auto emptyResults = service.search("");
    REQUIRE(emptyResults.empty());

    auto results = service.search("test");
    REQUIRE(results.size() == 1);
    REQUIRE(results[0].getTitle() == "MockItem");
}

class RankingMockSearcher : public Searcher
{
  public:
    RankingMockSearcher() {}
    virtual ~RankingMockSearcher() {}

    std::vector<SearchResult> search(const std::string& query) override
    {
        std::vector<SearchResult> res;
        if (query == "pref")
        {
            res.emplace_back("WindowItem", "Window", "Window description", nullptr);
            res.emplace_back("UIItem", "UI", "UI description", nullptr);
            res.emplace_back("PrefItem", "Preferences", "Pref description", nullptr);
        }
        return res;
    }
};

TEST_CASE("SearchService/RankingPreferencesBetweenUIAndWindow")
{
    SearchService service;
    service.addSearcher(std::make_unique<RankingMockSearcher>());

    auto results = service.search("pref");
    REQUIRE(results.size() == 3);
    REQUIRE(results[0].getCategory() == "UI");
    REQUIRE(results[1].getCategory() == "Preferences");
    REQUIRE(results[2].getCategory() == "Window");
}
#endif
