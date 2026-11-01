#pragma once

#include "Search/SearchMatcher.h"
#include "Search/SearchProvider.h"

#include <memory>
#include <string>

class Asset;
class GatedDatabase;
class Windows;

// Search provider querying project assets, metadata, geometry stats, and note attachments.
class AssetSearchProvider : public SearchProvider
{
  public:
    AssetSearchProvider(GatedDatabase& gatedDatabase, Windows& windows);
    virtual ~AssetSearchProvider();

    std::vector<SearchResult> search(const std::string& query) override;

  private:
    std::string buildAssetSummary(const std::shared_ptr<const Asset>& asset) const;

    SearchMatcher  matcher;
    GatedDatabase& gatedDatabase;
    Windows&       windows;
};
