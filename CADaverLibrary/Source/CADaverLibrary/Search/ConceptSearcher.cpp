#include "Search/ConceptSearcher.h"

#include "Concept/Concept.h"

ConceptSearcher::ConceptSearcher()
  : factory()
{
}

ConceptSearcher::ConceptSearcher(const ConceptFactory& factory)
  : factory(factory)
{
}

ConceptSearcher::~ConceptSearcher() {}

std::vector<SearchResult> ConceptSearcher::search(const std::string& query)
{
    std::vector<SearchResult> results;
    if (query.empty())
    {
        return results;
    }

    auto allConcepts = factory.getAllConcepts();
    for (const auto& conceptItem : allConcepts)
    {
        if (!conceptItem)
        {
            continue;
        }

        std::string type        = conceptItem->getType();
        std::string description = conceptItem->getDescription();

        if (matcher.matchesAny(query, type, description))
        {
            results.emplace_back(type, "Concept", description, nullptr);
        }
    }

    return results;
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("ConceptSearcher/SearchMatching")
{
    ConceptSearcher searcher;

    auto results = searcher.search("STEP");
    REQUIRE(!results.empty());
    REQUIRE(results[0].getCategory() == "Concept");

    auto brepResults = searcher.search("brep");
    REQUIRE(!brepResults.empty());
    REQUIRE(brepResults[0].getCategory() == "Concept");
}
#endif
