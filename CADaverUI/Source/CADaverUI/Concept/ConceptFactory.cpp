#include "Concept/ConceptFactory.h"

#include "Concept/StepFileConcept.h"
#include "Concept/StlFileConcept.h"
#include "Concept/SvoFileConcept.h"

ConceptFactory::ConceptFactory()
{
    registerConcepts();
}

ConceptFactory::~ConceptFactory() {}

std::vector<std::shared_ptr<Concept>> ConceptFactory::getAllConcepts() const
{
    return concepts;
}

void ConceptFactory::registerConcepts()
{
    concepts.push_back(std::make_shared<StepFileConcept>());
    concepts.push_back(std::make_shared<StlFileConcept>());
    concepts.push_back(std::make_shared<SvoFileConcept>());
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("ConceptFactory/Retrieval")
{
    ConceptFactory factory;
    auto           all = factory.getAllConcepts();
    REQUIRE(all.size() >= 3);
}
#endif
