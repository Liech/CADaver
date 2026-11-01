#pragma once

#include "Search/SearchMatcher.h"
#include "Search/SearchProvider.h"

class Windows;

// Search provider querying registered window types and descriptions from WindowFactory.
class WindowSearchProvider : public SearchProvider
{
  public:
    WindowSearchProvider(Windows& windows);
    virtual ~WindowSearchProvider();

    std::vector<SearchResult> search(const std::string& query) override;

  private:
    SearchMatcher matcher;
    Windows&      windows;
};
