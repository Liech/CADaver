#pragma once

#include "Search/SearchMatcher.h"
#include "Search/Searcher.h"

class Windows;

// Searcher querying window types and purpose descriptions from WindowFactory.
class WindowSearcher : public Searcher
{
  public:
    explicit WindowSearcher(Windows& windows);
    virtual ~WindowSearcher();

    std::vector<SearchResult> search(const std::string& query) override;

  private:
    SearchMatcher matcher;
    Windows&      windows;
};
