#pragma once

#include "Search/SearchResult.h"

#include <string>
#include <vector>

// Abstract base class for modular search sources.
class Searcher
{
  public:
    Searcher();
    virtual ~Searcher();

    virtual std::vector<SearchResult> search(const std::string& query) = 0;
};
