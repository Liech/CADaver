#pragma once

#include <atomic>
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

class McpServer;

// Localhost TCP socket server exposing McpServer communication over a local port.
class McpSocketServer
{
  public:
    McpSocketServer(McpServer& server);
    virtual ~McpSocketServer();

    bool start(int port = 0);
    void stop();

    int  getPort() const;
    bool isRunning() const;

  private:
    void acceptLoop();
    void handleClient(uint64_t clientSocket);

    McpServer&                   mcpServer;
    std::unique_ptr<std::thread> listenThread;
    std::mutex                   clientsMutex;
    std::vector<uint64_t>        activeClientSockets;
    uint64_t                     serverSocket = 0;
    int                          boundPort    = 0;
    std::atomic<bool>            running      = false;
};
