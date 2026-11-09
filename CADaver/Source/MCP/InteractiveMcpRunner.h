#pragma once

#include <atomic>
#include <memory>
#include <mutex>
#include <thread>

class CommandLineArgs;
class McpServer;

#include "MCP/McpPipeStream.h"

// Runner managing an interactive UI session driven concurrently by an MCP server.
class InteractiveMcpRunner
{
  public:
    InteractiveMcpRunner();
    virtual ~InteractiveMcpRunner();

    int run(const CommandLineArgs& args);

  private:
    void startMcpListener(McpServer& mcpServer);
    void stopMcpListener();

    McpPipeStream                pipeStream;
    std::unique_ptr<std::thread> mcpThread;
    std::mutex                   outputMutex;
    std::atomic<bool>            running = false;
};
