#include "Windows/Console/ConsoleStreamBuffer.h"

#include <utility>

ConsoleStreamBuffer::ConsoleStreamBuffer(std::ostream& targetStream, std::function<void(const std::string&)> onLine)
  : stream(targetStream)
  , oldBuffer(targetStream.rdbuf())
  , lineCallback(std::move(onLine))
{
    stream.rdbuf(this);
}

ConsoleStreamBuffer::~ConsoleStreamBuffer()
{
    sync();
    stream.rdbuf(oldBuffer);
}

ConsoleStreamBuffer::int_type ConsoleStreamBuffer::overflow(int_type c)
{
    if (c != traits_type::eof())
    {
        char character = static_cast<char>(c);
        if (oldBuffer)
        {
            oldBuffer->sputc(character);
        }

        std::lock_guard<std::mutex> lock(bufferMutex);
        if (character == '\n')
        {
            flushCurrentLine();
        }
        else if (character != '\r')
        {
            currentLine.push_back(character);
        }
    }

    return c;
}

int ConsoleStreamBuffer::sync()
{
    if (oldBuffer)
    {
        oldBuffer->pubsync();
    }

    std::lock_guard<std::mutex> lock(bufferMutex);
    if (!currentLine.empty())
    {
        flushCurrentLine();
    }

    return 0;
}

std::streamsize ConsoleStreamBuffer::xsputn(const char* s, std::streamsize count)
{
    if (oldBuffer)
    {
        oldBuffer->sputn(s, count);
    }

    std::lock_guard<std::mutex> lock(bufferMutex);
    for (std::streamsize i = 0; i < count; i++)
    {
        char character = s[i];
        if (character == '\n')
        {
            flushCurrentLine();
        }
        else if (character != '\r')
        {
            currentLine.push_back(character);
        }
    }

    return count;
}

void ConsoleStreamBuffer::flushCurrentLine()
{
    if (lineCallback)
    {
        lineCallback(currentLine);
    }

    currentLine.clear();
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>
#include <sstream>

TEST_CASE("ConsoleStreamBuffer/CapturesSingleLine")
{
    std::ostringstream ss;
    std::string        captured;
    {
        ConsoleStreamBuffer buffer(ss, [&](const std::string& line) { captured = line; });
        ss << "Hello World\n";
    }

    CHECK(captured == "Hello World");
    CHECK(ss.str() == "Hello World\n");
}

TEST_CASE("ConsoleStreamBuffer/CapturesMultipleLines")
{
    std::ostringstream       ss;
    std::vector<std::string> captured;
    {
        ConsoleStreamBuffer buffer(ss, [&](const std::string& line) { captured.push_back(line); });
        ss << "Line 1\nLine 2\r\nLine 3\n";
    }

    REQUIRE(captured.size() == 3);
    CHECK(captured[0] == "Line 1");
    CHECK(captured[1] == "Line 2");
    CHECK(captured[2] == "Line 3");
}

TEST_CASE("ConsoleStreamBuffer/SyncFlushesIncompleteLine")
{
    std::ostringstream ss;
    std::string        captured;
    {
        ConsoleStreamBuffer buffer(ss, [&](const std::string& line) { captured = line; });
        ss << "No newline";
        ss.flush();
        CHECK(captured == "No newline");
    }
}
#endif
