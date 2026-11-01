#include "Search/ConceptSearchProvider.h"

#include "Concept/Concept.h"
#include "Concept/ConceptFactory.h"
#include "Windows/Windows.h"

ConceptSearchProvider::ConceptSearchProvider(Windows& windows)
  : windows(windows)
{
}

ConceptSearchProvider::~ConceptSearchProvider() {}

std::vector<SearchResult> ConceptSearchProvider::search(const std::string& query)
{
    std::vector<SearchResult> results;
    if (query.empty())
    {
        return results;
    }

    ConceptFactory factory;
    auto           allConcepts = factory.getAllConcepts();

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
            results.emplace_back(type, "Concept", description, [this]() { windows.open("ConceptsInfoWindow"); });
        }
    }

    return results;
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("ConceptSearchProvider/SearchMatching")
{
    Windows               windows;
    ConceptSearchProvider provider(windows);

    auto results = provider.search("STEP");
    REQUIRE(!results.empty());
    REQUIRE(results[0].getCategory() == "Concept");

    auto brepResults = provider.search("brep");
    REQUIRE(!brepResults.empty());
    REQUIRE(brepResults[0].getCategory() == "Concept");
}
#endif
