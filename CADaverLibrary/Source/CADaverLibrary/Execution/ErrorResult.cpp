#include "Execution/ErrorResult.h"

#include <stdexcept>

ErrorResult::ErrorResult() {}

ErrorResult::ErrorResult(const std::string& command, const std::string& errorType, const std::string& message) : command(command), errorType(errorType), message(message) {}

ErrorResult::~ErrorResult() {}

const std::string& ErrorResult::getCommand() const
{
    return command;
}

void ErrorResult::setCommand(const std::string& commandInput)
{
    command = commandInput;
}

const std::string& ErrorResult::getErrorType() const
{
    return errorType;
}

void ErrorResult::setErrorType(const std::string& errorTypeInput)
{
    errorType = errorTypeInput;
}

const std::string& ErrorResult::getMessage() const
{
    return message;
}

void ErrorResult::setMessage(const std::string& messageInput)
{
    message = messageInput;
}

nlohmann::json ErrorResult::toJson() const
{
    return {
        {      "type",   "Error" },
        {   "version",         1 },
        {   "command",   command },
        { "errorType", errorType },
        {   "message",   message }
    };
}

void ErrorResult::fromJson(const nlohmann::json& j)
{
    int version = j.at("version").get<int>();
    if (version != 1)
    {
        throw std::runtime_error("Unsupported ErrorResult version");
    }

    command   = j.at("command").get<std::string>();
    errorType = j.at("errorType").get<std::string>();
    message   = j.at("message").get<std::string>();
}

Polyglot::Node ErrorResult::toNode() const
{
    return Polyglot::Node(toJson());
}

void ErrorResult::fromNode(const Polyglot::Node& node)
{
    fromJson(node.toJson());
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("ErrorResult/SerializeRoundTrip")
{
    ErrorResult err("LoadAsset", "LoadFailed", "Failed to parse asset stream");
    CHECK(err.getCommand() == "LoadAsset");
    CHECK(err.getErrorType() == "LoadFailed");
    CHECK(err.getMessage() == "Failed to parse asset stream");

    nlohmann::json j = err.toJson();
    CHECK(j.at("type") == "Error");
    CHECK(j.at("version") == 1);
    CHECK(j.at("command") == "LoadAsset");
    CHECK(j.at("errorType") == "LoadFailed");
    CHECK(j.at("message") == "Failed to parse asset stream");

    ErrorResult restored;
    restored.fromJson(j);
    CHECK(restored.getCommand() == "LoadAsset");
    CHECK(restored.getErrorType() == "LoadFailed");
    CHECK(restored.getMessage() == "Failed to parse asset stream");
}
#endif
