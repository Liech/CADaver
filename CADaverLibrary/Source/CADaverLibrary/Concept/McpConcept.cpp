#include "Concept/McpConcept.h"

McpConcept::McpConcept() {}

McpConcept::~McpConcept() {}

std::string McpConcept::getType() const
{
    return "MCP";
}

std::string McpConcept::getDescription() const
{
    return "MCP (Model Context Protocol) enables AI assistants and external tools to inspect and control CADaver over JSON-RPC 2.0.\n\n"
           "Execution Modes:\n"
           "  - Interactive Mode (--mcp): Launches the full CADaver GUI and viewport while listening for MCP requests on a background thread. "
           "Incoming tool calls are dispatched onto the main thread via Defer so geometry changes, notes, and preferences update live in the viewport.\n"
           "  - Headless Mode (--mcp --headless): Runs the MCP server directly against a headless GatedDatabase without creating a window or graphics device.\n\n"
           "Transports:\n"
           "  - Standard I/O (stdio): Default line-delimited JSON-RPC transport using dedicated pipe handles.\n"
           "  - TCP Socket (--port <number> / -p <number>): Optional TCP listener accepting line-delimited JSON-RPC connections.\n\n"
           "Exposed Tools:\n"
           "  - cadaver_list_commands: Lists all registered commands, versions, and descriptions.\n"
           "  - cadaver_get_documentation: Queries full documentation for any Command, Concept, Asset, or Note.\n"
           "  - cadaver_execute_command: Executes any command by type name and JSON arguments.\n"
           "  - Direct Command Tools: Every command registered in CommandFactory (e.g. GenerateLandscape, LoadStlAsset, ConvertCadToSvo, AddNote) is also exposed as a direct MCP tool.";
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("McpConcept/TypeAndDescription")
{
    McpConcept item;
    REQUIRE(item.getType() == "MCP");
    REQUIRE(!item.getDescription().empty());
}
#endif
