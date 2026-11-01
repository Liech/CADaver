#pragma once

#include "Search/SearchMatcher.h"
#include "Search/SearchProvider.h"

class Windows;

// Search provider querying conceptual documentation and file format specifications.
class ConceptSearchProvider : public SearchProvider
{
  public:
    ConceptSearchProvider(Windows& windows);
    virtual ~ConceptSearchProvider();

    std::vector<SearchResult> search(const std::string& query) override;

  private:
    SearchMatcher matcher;
    Windows&      windows;
};
