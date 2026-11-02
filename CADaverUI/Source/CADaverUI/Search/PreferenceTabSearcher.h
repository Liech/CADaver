#pragma once

#include "Search/SearchMatcher.h"
#include "Search/Searcher.h"

class Windows;

// Searcher querying preference tabs and their settings descriptions.
class PreferenceTabSearcher : public Searcher
{
  public:
    explicit PreferenceTabSearcher(Windows& windows);
    virtual ~PreferenceTabSearcher();

    std::vector<SearchResult> search(const std::string& query) override;

  private:
    SearchMatcher matcher;
    Windows&      windows;
};
