#pragma once

#include "Search/SearchMatcher.h"
#include "Search/SearchProvider.h"

class GatedDatabase;
class Windows;

// Search provider querying command types and descriptions from CommandFactory.
class CommandSearchProvider : public SearchProvider
{
  public:
    CommandSearchProvider(GatedDatabase& gatedDatabase, Windows& windows);
    virtual ~CommandSearchProvider();

    std::vector<SearchResult> search(const std::string& query) override;

  private:
    SearchMatcher  matcher;
    GatedDatabase& gatedDatabase;
    Windows&       windows;
};
