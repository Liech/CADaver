#include "Concept/CommandConcept.h"

CommandConcept::CommandConcept() {}

CommandConcept::~CommandConcept() {}

std::string CommandConcept::getType() const
{
    return "Command";
}

std::string CommandConcept::getDescription() const
{
    return "A Command represents an executable user action or operation, such as loading a file, "
           "generating geometry, or modifying a property. Commands manage execution state and support undo and redo operations.";
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("CommandConcept/TypeAndDescription")
{
    CommandConcept item;
    REQUIRE(item.getType() == "Command");
    REQUIRE(!item.getDescription().empty());
}
#endif
