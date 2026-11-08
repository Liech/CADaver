#pragma once

#include "Search/SearchMatcher.h"
#include "Search/Searcher.h"

class CommandFactory;

// Searcher querying command types and descriptions from CommandFactory.
class CommandSearcher : public Searcher
{
  public:
    explicit CommandSearcher(const CommandFactory& commandFactory);
    virtual ~CommandSearcher();

    std::vector<SearchResult> search(const std::string& query) override;

  private:
    SearchMatcher         matcher;
    const CommandFactory& commandFactory;
};
