#pragma once

#include <string>

// Cross-platform standard IO stream handler supporting Windows pipe handles.
class McpPipeStream
{
  public:
    McpPipeStream();
    virtual ~McpPipeStream();

    bool readLine(std::string& line);
    void writeLine(const std::string& line);

  private:
    std::string inputBuffer;
    void*       inputHandle  = nullptr;
    void*       outputHandle = nullptr;
};
