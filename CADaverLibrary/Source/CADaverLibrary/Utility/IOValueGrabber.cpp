#include "Utility/IOValueGrabber.h"

#include <fstream>

IOValueGrabber::IOValueGrabber() {}

IOValueGrabber::~IOValueGrabber() {}

bool IOValueGrabber::isFilePath(const Polyglot::Node& node)
{
    return node.isString();
}

bool IOValueGrabber::isFunction(const Polyglot::Node& node)
{
    return node.isFunction();
}

bool IOValueGrabber::isBinary(const Polyglot::Node& node)
{
    return node.isBinary();
}

std::vector<uint8_t> IOValueGrabber::grabBytes(const Polyglot::Node& source)
{
    if (source.isFunction())
    {
        Polyglot::Node callbackResult = source.asFunction()(Polyglot::Node());
        if (callbackResult.isBinary())
        {
            return callbackResult.asBinary();
        }
        if (callbackResult.isString())
        {
            const std::string& stringData = callbackResult.asString();
            return std::vector<uint8_t>(stringData.begin(), stringData.end());
        }
        return {};
    }

    if (source.isBinary())
    {
        return source.asBinary();
    }

    if (source.isString())
    {
        const std::string& filePath = source.asString();
        std::ifstream      stream(filePath, std::ios::binary | std::ios::ate);
        if (stream.is_open())
        {
            std::streamsize      fileSize = stream.tellg();
            std::vector<uint8_t> fileBytes(static_cast<size_t>(fileSize));
            stream.seekg(0, std::ios::beg);
            stream.read(static_cast<char*>(static_cast<void*>(fileBytes.data())), fileSize);
            return fileBytes;
        }
    }

    return {};
}

std::string IOValueGrabber::grabFilePath(const Polyglot::Node& node)
{
    if (node.isString())
    {
        return node.asString();
    }
    return "";
}

bool IOValueGrabber::deliverBytes(const Polyglot::Node& destination, const std::vector<uint8_t>& bytes)
{
    if (destination.isFunction())
    {
        destination.asFunction()(Polyglot::Node(bytes));
        return true;
    }

    if (destination.isString())
    {
        const std::string& filePath = destination.asString();
        if (!filePath.empty())
        {
            std::ofstream stream(filePath, std::ios::binary);
            if (stream.is_open())
            {
                stream.write(static_cast<const char*>(static_cast<const void*>(bytes.data())), bytes.size());
                return true;
            }
        }
    }

    return false;
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>
#include <filesystem>

TEST_CASE("IOValueGrabber/TypeQueries")
{
    Polyglot::Node stringNode("test.stl");
    CHECK(IOValueGrabber::isFilePath(stringNode));
    CHECK_FALSE(IOValueGrabber::isFunction(stringNode));
    CHECK_FALSE(IOValueGrabber::isBinary(stringNode));

    std::vector<uint8_t> bytes = { 0x01, 0x02 };
    Polyglot::Node       binaryNode(bytes);
    CHECK(IOValueGrabber::isBinary(binaryNode));
    CHECK_FALSE(IOValueGrabber::isFilePath(binaryNode));

    std::function<Polyglot::Node(const Polyglot::Node&)> callback = [](const Polyglot::Node& n) -> Polyglot::Node { return n; };
    Polyglot::Node                                       funcNode(callback);
    CHECK(IOValueGrabber::isFunction(funcNode));
    CHECK_FALSE(IOValueGrabber::isFilePath(funcNode));
}

TEST_CASE("IOValueGrabber/GrabBytesFromBinary")
{
    std::vector<uint8_t> bytes = { 0xAA, 0xBB, 0xCC };
    Polyglot::Node       node(bytes);
    std::vector<uint8_t> result = IOValueGrabber::grabBytes(node);
    CHECK(result == bytes);
}

TEST_CASE("IOValueGrabber/GrabBytesFromFunctionBinary")
{
    std::vector<uint8_t>                                 bytes    = { 0x10, 0x20, 0x30 };
    std::function<Polyglot::Node(const Polyglot::Node&)> callback = [bytes](const Polyglot::Node&) -> Polyglot::Node { return Polyglot::Node(bytes); };
    Polyglot::Node                                       node(callback);
    std::vector<uint8_t>                                 result = IOValueGrabber::grabBytes(node);
    CHECK(result == bytes);
}

TEST_CASE("IOValueGrabber/GrabBytesFromFunctionString")
{
    std::string                                          sample   = "ABC";
    std::function<Polyglot::Node(const Polyglot::Node&)> callback = [sample](const Polyglot::Node&) -> Polyglot::Node { return Polyglot::Node(sample); };
    Polyglot::Node                                       node(callback);
    std::vector<uint8_t>                                 result = IOValueGrabber::grabBytes(node);
    std::string                                          asStr(result.begin(), result.end());
    CHECK(asStr == sample);
}

TEST_CASE("IOValueGrabber/GrabFilePath")
{
    Polyglot::Node node("path/to/file.step");
    CHECK(IOValueGrabber::grabFilePath(node) == "path/to/file.step");

    Polyglot::Node notString(123);
    CHECK(IOValueGrabber::grabFilePath(notString).empty());
}

TEST_CASE("IOValueGrabber/DeliverBytesToFunction")
{
    std::vector<uint8_t>                                 received;
    std::function<Polyglot::Node(const Polyglot::Node&)> callback = [&received](const Polyglot::Node& n) -> Polyglot::Node
    {
        received = n.asBinary();
        return Polyglot::Node();
    };
    Polyglot::Node dest(callback);

    std::vector<uint8_t> bytes = { 0x05, 0x06 };
    bool                 ok    = IOValueGrabber::deliverBytes(dest, bytes);
    CHECK(ok);
    CHECK(received == bytes);
}

TEST_CASE("IOValueGrabber/DeliverBytesToFile")
{
    std::string          tempPath = (std::filesystem::temp_directory_path() / "iovg_test.bin").string();
    std::vector<uint8_t> bytes    = { 0xDE, 0xAD };

    Polyglot::Node dest(tempPath);
    bool           ok = IOValueGrabber::deliverBytes(dest, bytes);
    CHECK(ok);

    std::vector<uint8_t> readBack = IOValueGrabber::grabBytes(dest);
    CHECK(readBack == bytes);

    std::filesystem::remove(tempPath);
}
#endif
