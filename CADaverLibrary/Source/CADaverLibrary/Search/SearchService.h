#pragma once

#include "Search/SearchResult.h"
#include "Search/Searcher.h"

#include <memory>
#include <string>
#include <vector>

// Coordinates modular search sources and executes aggregated queries.
class SearchService
{
  public:
    SearchService();
    virtual ~SearchService();

    void addSearcher(std::unique_ptr<Searcher> searcher);
    void clearSearchers();

    std::vector<SearchResult> search(const std::string& query) const;

  private:
    int getCategoryRank(const std::string& category) const;

    std::vector<std::unique_ptr<Searcher>> searchers;
};
