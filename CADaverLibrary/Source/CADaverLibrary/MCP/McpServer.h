#pragma once

#include <functional>
#include <iosfwd>
#include <nlohmann/json.hpp>
#include <string>

class GatedDatabase;

// Server implementing Model Context Protocol (MCP) JSON-RPC 2.0 communication for GatedDatabase.
class McpServer
{
  public:
    McpServer(GatedDatabase& database);
    virtual ~McpServer();

    std::string handleMessage(const std::string& rawJsonRpc);
    void        run(std::istream& in, std::ostream& out);

    void setCommandExecutor(std::function<nlohmann::json(const nlohmann::json&)> executor);

  private:
    nlohmann::json handleInitialize(const nlohmann::json& request);
    nlohmann::json handlePing(const nlohmann::json& request);
    nlohmann::json handleToolsList(const nlohmann::json& request);
    nlohmann::json handleToolsCall(const nlohmann::json& request);

    nlohmann::json executeJsonCommand(const nlohmann::json& command);

    nlohmann::json createErrorResponse(const nlohmann::json& id, int code, const std::string& message) const;
    nlohmann::json createSuccessResponse(const nlohmann::json& id, const nlohmann::json& result) const;

    GatedDatabase&                                       gatedDatabase;
    std::function<nlohmann::json(const nlohmann::json&)> commandExecutor;
};
