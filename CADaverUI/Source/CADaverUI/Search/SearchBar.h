#pragma once

#include "Search/SearchResult.h"
#include "Search/SearchService.h"

#include <string>
#include <utility>
#include <vector>

class Defer;
class GatedDatabase;
class UIElementFactory;
class Windows;

// Renders the sleek top-menu search bar with interactive dropdown results.
class SearchBar
{
  public:
    SearchBar();
    virtual ~SearchBar();

    void init(GatedDatabase& gatedDatabase, Windows& windows, UIElementFactory& uiElementFactory);
    void render(Defer& defer);

    void focus();
    bool isFocusRequested() const;

  private:
    void                                   renderSearchIcon() const;
    void                                   renderDropdown(Defer& defer, float availableHeight);
    void                                   renderResultCard(size_t index, const SearchResult& item, bool isSelected, const std::vector<std::string>& queryTokens, Defer& defer);
    void                                   renderHighlightedLine(const std::string& line, const std::vector<std::string>& queryTokens) const;
    void                                   executeSelection(size_t index, Defer& defer);
    std::vector<std::string>               extractContextLines(const std::string& text, const std::vector<std::string>& queryTokens, size_t maxLines) const;
    std::vector<std::pair<size_t, size_t>> findMatchRanges(const std::string& text, const std::vector<std::string>& queryTokens) const;
    std::vector<std::string>               tokenizeQuery(const std::string& text) const;
    std::string                            toLower(const std::string& input) const;

    SearchService             searchService;
    std::string               query;
    std::string               lastQuery;
    std::vector<SearchResult> results;
    int                       selectedIndex  = -1;
    bool                      initialized    = false;
    bool                      isOpen         = false;
    bool                      focusRequested = false;
    Windows*                  windows        = nullptr;
};
