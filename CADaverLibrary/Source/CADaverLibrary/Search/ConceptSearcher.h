#pragma once

#include "Concept/ConceptFactory.h"
#include "Search/SearchMatcher.h"
#include "Search/Searcher.h"

// Searcher querying conceptual documentation and file format specifications.
class ConceptSearcher : public Searcher
{
  public:
    ConceptSearcher();
    explicit ConceptSearcher(const ConceptFactory& factory);
    virtual ~ConceptSearcher();

    std::vector<SearchResult> search(const std::string& query) override;

  private:
    SearchMatcher  matcher;
    ConceptFactory factory;
};
