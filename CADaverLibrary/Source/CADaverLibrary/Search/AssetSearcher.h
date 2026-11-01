#pragma once

#include "Search/SearchMatcher.h"
#include "Search/Searcher.h"

#include <memory>
#include <string>

class Asset;
class Database;

// Searcher querying project assets, metadata, geometry stats, and note attachments.
class AssetSearcher : public Searcher
{
  public:
    explicit AssetSearcher(const Database& database);
    virtual ~AssetSearcher();

    std::vector<SearchResult> search(const std::string& query) override;

  private:
    std::string buildAssetSummary(const std::shared_ptr<const Asset>& asset) const;

    SearchMatcher   matcher;
    const Database& database;
};
