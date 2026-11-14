#pragma once

#include "Polyglot/Node.h"

#include <nlohmann/json.hpp>
#include <string>

// Represents a standardized error response for failed command executions.
class ErrorResult
{
  public:
    ErrorResult();
    ErrorResult(const std::string& command, const std::string& errorType, const std::string& message);
    virtual ~ErrorResult();

    const std::string& getCommand() const;
    void               setCommand(const std::string& command);

    const std::string& getErrorType() const;
    void               setErrorType(const std::string& errorType);

    const std::string& getMessage() const;
    void               setMessage(const std::string& message);

    nlohmann::json toJson() const;
    void           fromJson(const nlohmann::json& j);

    Polyglot::Node toNode() const;
    void           fromNode(const Polyglot::Node& node);

  private:
    std::string command;
    std::string errorType;
    std::string message;
};
