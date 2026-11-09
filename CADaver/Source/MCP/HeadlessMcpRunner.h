#pragma once

class CommandLineArgs;

// Headless executor running the Model Context Protocol (MCP) server over standard input and output.
class HeadlessMcpRunner
{
  public:
    HeadlessMcpRunner();
    virtual ~HeadlessMcpRunner();

    int run(const CommandLineArgs& args);
};
