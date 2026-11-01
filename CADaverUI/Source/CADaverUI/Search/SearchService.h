#pragma once

#include "Search/SearchProvider.h"
#include "Search/SearchResult.h"

#include <memory>
#include <string>
#include <vector>

// Coordinates modular search providers and executes aggregated queries.
class SearchService
{
  public:
    SearchService();
    virtual ~SearchService();

    void addProvider(std::unique_ptr<SearchProvider> provider);
    void clearProviders();

    std::vector<SearchResult> search(const std::string& query) const;

  private:
    int getCategoryRank(const std::string& category) const;

    std::vector<std::unique_ptr<SearchProvider>> providers;
};
