#include "MCP/McpSocketServer.h"

#include "MCP/McpServer.h"

#include <algorithm>
#include <iostream>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

union SocketAddress
{
    sockaddr_in in;
    sockaddr    generic;
};

McpSocketServer::McpSocketServer(McpServer& server)
  : mcpServer(server)
{
#ifdef _WIN32
    WSADATA wsaData{};
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
}

McpSocketServer::~McpSocketServer()
{
    stop();

#ifdef _WIN32
    WSACleanup();
#endif
}

bool McpSocketServer::start(int port)
{
    if (running)
    {
        return true;
    }

#ifdef _WIN32
    SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == INVALID_SOCKET)
    {
        return false;
    }

    char reuse = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    SocketAddress addr{};
    addr.in.sin_family      = AF_INET;
    addr.in.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.in.sin_port        = htons(static_cast<u_short>(port));

    if (bind(s, &addr.generic, sizeof(addr.in)) == SOCKET_ERROR)
    {
        closesocket(s);
        return false;
    }

    if (listen(s, SOMAXCONN) == SOCKET_ERROR)
    {
        closesocket(s);
        return false;
    }

    int           addrLen = sizeof(addr.in);
    SocketAddress boundAddr{};
    if (getsockname(s, &boundAddr.generic, &addrLen) != SOCKET_ERROR)
    {
        boundPort = ntohs(boundAddr.in.sin_port);
    }
    else
    {
        boundPort = port;
    }

    serverSocket = static_cast<uint64_t>(s);
#else
    int s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0)
    {
        return false;
    }

    int reuse = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    SocketAddress addr{};
    addr.in.sin_family      = AF_INET;
    addr.in.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.in.sin_port        = htons(static_cast<uint16_t>(port));

    if (bind(s, &addr.generic, sizeof(addr.in)) < 0)
    {
        close(s);
        return false;
    }

    if (listen(s, SOMAXCONN) < 0)
    {
        close(s);
        return false;
    }

    socklen_t     addrLen = sizeof(addr.in);
    SocketAddress boundAddr{};
    if (getsockname(s, &boundAddr.generic, &addrLen) == 0)
    {
        boundPort = ntohs(boundAddr.in.sin_port);
    }
    else
    {
        boundPort = port;
    }

    serverSocket = static_cast<uint64_t>(s);
#endif

    running      = true;
    listenThread = std::make_unique<std::thread>([this]() { acceptLoop(); });
    return true;
}

void McpSocketServer::stop()
{
    if (!running)
    {
        return;
    }

    running = false;

    if (serverSocket != 0)
    {
#ifdef _WIN32
        closesocket(static_cast<SOCKET>(serverSocket));
#else
        close(static_cast<int>(serverSocket));
#endif
        serverSocket = 0;
    }

    {
        std::lock_guard<std::mutex> lock(clientsMutex);
        for (uint64_t client : activeClientSockets)
        {
#ifdef _WIN32
            closesocket(static_cast<SOCKET>(client));
#else
            close(static_cast<int>(client));
#endif
        }
        activeClientSockets.clear();
    }

    if (listenThread && listenThread->joinable())
    {
        listenThread->join();
    }
    listenThread.reset();
}

int McpSocketServer::getPort() const
{
    return boundPort;
}

bool McpSocketServer::isRunning() const
{
    return running;
}

void McpSocketServer::acceptLoop()
{
    while (running)
    {
        SocketAddress clientAddr{};
#ifdef _WIN32
        int    addrLen = sizeof(clientAddr.in);
        SOCKET client  = accept(static_cast<SOCKET>(serverSocket), &clientAddr.generic, &addrLen);
        if (client == INVALID_SOCKET)
        {
            break;
        }
#else
        socklen_t addrLen = sizeof(clientAddr.in);
        int       client  = accept(static_cast<int>(serverSocket), &clientAddr.generic, &addrLen);
        if (client < 0)
        {
            break;
        }
#endif

        uint64_t clientSocket = static_cast<uint64_t>(client);
        {
            std::lock_guard<std::mutex> lock(clientsMutex);
            activeClientSockets.push_back(clientSocket);
        }

        std::thread clientThread([this, clientSocket]() { handleClient(clientSocket); });
        clientThread.detach();
    }
}

void McpSocketServer::handleClient(uint64_t clientSocket)
{
    std::string buffer;
    char        chunk[1024];

    while (running)
    {
#ifdef _WIN32
        int bytes = recv(static_cast<SOCKET>(clientSocket), chunk, sizeof(chunk), 0);
#else
        ssize_t bytes = recv(static_cast<int>(clientSocket), chunk, sizeof(chunk), 0);
#endif
        if (bytes <= 0)
        {
            break;
        }

        buffer.append(chunk, static_cast<size_t>(bytes));

        size_t newlinePos = buffer.find('\n');
        while (newlinePos != std::string::npos)
        {
            std::string line = buffer.substr(0, newlinePos);
            buffer.erase(0, newlinePos + 1);

            if (!line.empty() && line.back() == '\r')
            {
                line.pop_back();
            }

            if (!line.empty())
            {
                std::string response = mcpServer.handleMessage(line);
                if (!response.empty())
                {
                    response += "\n";
#ifdef _WIN32
                    send(static_cast<SOCKET>(clientSocket), response.data(), static_cast<int>(response.size()), 0);
#else
                    send(static_cast<int>(clientSocket), response.data(), response.size(), 0);
#endif
                }
            }

            newlinePos = buffer.find('\n');
        }
    }

#ifdef _WIN32
    closesocket(static_cast<SOCKET>(clientSocket));
#else
    close(static_cast<int>(clientSocket));
#endif

    std::lock_guard<std::mutex> lock(clientsMutex);
    auto                        it = std::find(activeClientSockets.begin(), activeClientSockets.end(), clientSocket);
    if (it != activeClientSockets.end())
    {
        activeClientSockets.erase(it);
    }
}

#ifdef ISTESTPROJECT
#include "Execution/GatedDatabase.h"
#include <catch2/catch_test_macros.hpp>
#include <nlohmann/json.hpp>

TEST_CASE("McpSocketServer/LifecycleAndPing")
{
    GatedDatabase   database;
    McpServer       mcpServer(database);
    McpSocketServer socketServer(mcpServer);

    REQUIRE(socketServer.start(0));
    REQUIRE(socketServer.isRunning());
    int port = socketServer.getPort();
    REQUIRE(port > 0);

#ifdef _WIN32
    SOCKET client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    REQUIRE(client != INVALID_SOCKET);

    SocketAddress targetAddr{};
    targetAddr.in.sin_family      = AF_INET;
    targetAddr.in.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    targetAddr.in.sin_port        = htons(static_cast<u_short>(port));

    int connectRes = connect(client, &targetAddr.generic, sizeof(targetAddr.in));
    REQUIRE(connectRes != SOCKET_ERROR);

    std::string pingReq = "{\"jsonrpc\":\"2.0\",\"id\":100,\"method\":\"ping\"}\n";
    send(client, pingReq.data(), static_cast<int>(pingReq.size()), 0);

    char recvBuf[1024];
    int  bytes = recv(client, recvBuf, sizeof(recvBuf) - 1, 0);
    REQUIRE(bytes > 0);
    recvBuf[bytes] = '\0';

    nlohmann::json responseJson = nlohmann::json::parse(recvBuf);
    CHECK(responseJson["id"] == 100);
    CHECK(responseJson["result"].is_object());

    closesocket(client);
#endif

    socketServer.stop();
    CHECK(!socketServer.isRunning());
}
#endif
