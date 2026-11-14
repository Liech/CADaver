#include "MCP/McpPipeStream.h"

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

McpPipeStream::McpPipeStream()
{
#ifdef _WIN32
    HANDLE currentProcess = GetCurrentProcess();
    HANDLE rawIn          = GetStdHandle(STD_INPUT_HANDLE);
    if (rawIn != INVALID_HANDLE_VALUE && rawIn != nullptr)
    {
        HANDLE dupIn = nullptr;
        if (DuplicateHandle(currentProcess, rawIn, currentProcess, &dupIn, 0, FALSE, DUPLICATE_SAME_ACCESS))
        {
            inputHandle = dupIn;
        }
    }

    HANDLE rawOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (rawOut != INVALID_HANDLE_VALUE && rawOut != nullptr)
    {
        HANDLE dupOut = nullptr;
        if (DuplicateHandle(currentProcess, rawOut, currentProcess, &dupOut, 0, FALSE, DUPLICATE_SAME_ACCESS))
        {
            outputHandle = dupOut;
        }
    }
#endif
}

McpPipeStream::~McpPipeStream()
{
#ifdef _WIN32
    if (inputHandle != nullptr)
    {
        CloseHandle(static_cast<HANDLE>(inputHandle));
        inputHandle = nullptr;
    }
    if (outputHandle != nullptr)
    {
        CloseHandle(static_cast<HANDLE>(outputHandle));
        outputHandle = nullptr;
    }
#endif
}

bool McpPipeStream::readLine(std::string& line)
{
#ifdef _WIN32
    HANDLE hIn = static_cast<HANDLE>(inputHandle);
    if (hIn != INVALID_HANDLE_VALUE && hIn != nullptr)
    {
        while (true)
        {
            size_t newlinePos = inputBuffer.find('\n');
            if (newlinePos != std::string::npos)
            {
                line = inputBuffer.substr(0, newlinePos);
                inputBuffer.erase(0, newlinePos + 1);
                if (!line.empty() && line.back() == '\r')
                {
                    line.pop_back();
                }
                return true;
            }

            char  chunk[1024];
            DWORD bytesRead = 0;
            if (!ReadFile(hIn, chunk, sizeof(chunk), &bytesRead, nullptr) || bytesRead == 0)
            {
                if (!inputBuffer.empty())
                {
                    line = inputBuffer;
                    inputBuffer.clear();
                    if (!line.empty() && line.back() == '\r')
                    {
                        line.pop_back();
                    }
                    return true;
                }
                return false;
            }
            inputBuffer.append(chunk, bytesRead);
        }
    }
#endif
    return static_cast<bool>(std::getline(std::cin, line));
}

void McpPipeStream::writeLine(const std::string& line)
{
#ifdef _WIN32
    HANDLE hOut = static_cast<HANDLE>(outputHandle);
    if (hOut != INVALID_HANDLE_VALUE && hOut != nullptr)
    {
        std::string fullMessage  = line + "\n";
        DWORD       bytesWritten = 0;
        WriteFile(hOut, fullMessage.data(), static_cast<DWORD>(fullMessage.size()), &bytesWritten, nullptr);
        return;
    }
#endif
    std::cout << line << "\n";
    std::cout.flush();
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("McpPipeStream/Construction")
{
    McpPipeStream stream;
    SUCCEED();
}
#endif
