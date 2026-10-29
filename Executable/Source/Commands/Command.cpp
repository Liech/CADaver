#include "Command.h"

Command::Command() {}

Command::~Command() {}

std::vector<CADacomb::GUID> Command::getRequiredAssets() const
{
    return {};
}

nlohmann::json Command::createResponse(const std::string& status, const nlohmann::json& payload) const
{
    nlohmann::json result = {
        {    "type",    getType() },
        { "version", getVersion() },
        {  "status",       status }
    };
    for (auto it = payload.begin(); it != payload.end(); ++it)
    {
        result[it.key()] = it.value();
    }
    return result;
}
