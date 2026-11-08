#pragma once

#include "Search/SearchMatcher.h"
#include "Search/Searcher.h"
#include "UIElement/UIElementFactory.h"

// Searcher querying registered UI elements and executing their activation callbacks.
class UIElementSearcher : public Searcher
{
  public:
    explicit UIElementSearcher(UIElementFactory& factory);
    virtual ~UIElementSearcher();

    std::vector<SearchResult> search(const std::string& query) override;

  private:
    SearchMatcher     matcher;
    UIElementFactory& factory;
};
