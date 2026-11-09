#include "Concept/ConceptFactory.h"

#include "Concept/AssetConcept.h"
#include "Concept/CommandConcept.h"
#include "Concept/CommandLineConcept.h"
#include "Concept/McpConcept.h"
#include "Concept/StepFileConcept.h"
#include "Concept/StlFileConcept.h"
#include "Concept/SubAssetConcept.h"
#include "Concept/SvoFileConcept.h"

ConceptFactory::ConceptFactory()
{
    registerConcepts();
}

ConceptFactory::~ConceptFactory() {}

std::shared_ptr<Concept> ConceptFactory::create(const std::string& type) const
{
    for (const auto& item : concepts)
    {
        if (item && item->getType() == type)
        {
            return item;
        }
    }
    return nullptr;
}

std::vector<std::shared_ptr<Concept>> ConceptFactory::getAllConcepts() const
{
    return concepts;
}

void ConceptFactory::registerConcepts()
{
    concepts.push_back(std::make_shared<StepFileConcept>());
    concepts.push_back(std::make_shared<StlFileConcept>());
    concepts.push_back(std::make_shared<SvoFileConcept>());
    concepts.push_back(std::make_shared<CommandConcept>());
    concepts.push_back(std::make_shared<CommandLineConcept>());
    concepts.push_back(std::make_shared<McpConcept>());
    concepts.push_back(std::make_shared<AssetConcept>());
    concepts.push_back(std::make_shared<SubAssetConcept>());
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("ConceptFactory/Retrieval")
{
    ConceptFactory factory;
    auto           all = factory.getAllConcepts();
    REQUIRE(all.size() >= 8);
    REQUIRE(factory.create("STEP") != nullptr);
    REQUIRE(factory.create("Command") != nullptr);
    REQUIRE(factory.create("CommandLine") != nullptr);
    REQUIRE(factory.create("MCP") != nullptr);
    REQUIRE(factory.create("Asset") != nullptr);
    REQUIRE(factory.create("SubAsset") != nullptr);
    REQUIRE(factory.create("Unknown") == nullptr);
}
#endif
