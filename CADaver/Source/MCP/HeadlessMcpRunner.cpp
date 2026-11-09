#include "MCP/HeadlessMcpRunner.h"

#include "CommandLineArgs.h"
#include "Execution/GatedDatabase.h"
#include "MCP/McpPipeStream.h"
#include "MCP/McpServer.h"
#include "MCP/McpSocketServer.h"
#include "Windows/Console/SystemConsole.h"

#include <cstdlib>
#include <iostream>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

HeadlessMcpRunner::HeadlessMcpRunner() {}

HeadlessMcpRunner::~HeadlessMcpRunner() {}

int HeadlessMcpRunner::run(const CommandLineArgs& args)
{
    SystemConsole::setAvailable(false);

    GatedDatabase gatedDatabase;
    if (args.isVerbose())
    {
        gatedDatabase.setLogCallback([](const std::string& msg) { std::cerr << msg << std::endl; });
    }
    else
    {
        gatedDatabase.setLogCallback([](const std::string&) {});
    }

    McpServer server(gatedDatabase);

    std::unique_ptr<McpSocketServer> socketServer;
    if (args.getPort() > 0)
    {
        socketServer = std::make_unique<McpSocketServer>(server);
        if (socketServer->start(args.getPort()))
        {
            std::cerr << "MCP Headless server listening on port " << socketServer->getPort() << std::endl;
        }
    }

    McpPipeStream pipeStream;
    std::string   line;
    while (pipeStream.readLine(line))
    {
        if (line.empty())
        {
            continue;
        }

        std::string response = server.handleMessage(line);
        if (!response.empty())
        {
            pipeStream.writeLine(response);
        }
    }

    return EXIT_SUCCESS;
}
