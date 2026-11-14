#include "Concept/CommandLineConcept.h"

CommandLineConcept::CommandLineConcept() {}

CommandLineConcept::~CommandLineConcept() {}

std::string CommandLineConcept::getType() const
{
    return "CommandLine";
}

std::string CommandLineConcept::getDescription() const
{
    return "CadaverExe supports command line arguments for interactive GUI sessions, headless script automation, and Model Context Protocol (MCP) server operation.\n\n"
           "Arguments:\n"
           "  - --script <path>, -s <path>: Executes a Lua or Python script file headlessly and exits.\n"
           "  - --lang <lua|python>, -l <lua|python>: Explicitly selects the scripting engine language (inferred from .lua or .py extension when omitted).\n"
           "  - --output <path>, -o <path>: Writes script execution status and error details as JSON to the specified file path.\n"
           "  - --mcp: Starts the Model Context Protocol (MCP) JSON-RPC server (launches interactive GUI mode unless --headless is specified).\n"
           "  - --headless: Runs the MCP server in headless mode without initializing Vulkan or opening a GUI window.\n"
           "  - --port <number>, -p <number>: Starts the MCP TCP socket server on the given port (automatically enables --mcp).\n"
           "  - --verbose, -v: Prints executed command inputs (IN) and outputs (OUT) to standard error (stderr).";
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("CommandLineConcept/TypeAndDescription")
{
    CommandLineConcept item;
    REQUIRE(item.getType() == "CommandLine");
    REQUIRE(!item.getDescription().empty());
}
#endif
