#include "Execution/Command.h"

#include "Execution/ErrorResult.h"

Command::Command() {}

Command::~Command() {}

std::vector<CADacomb::GUID> Command::getRequiredAssets() const
{
    return {};
}

void Command::setInput(const Polyglot::Node&) {}

bool Command::isUndoable() const
{
    return false;
}

void Command::undo(Database&) {}

void Command::redo(Database&) {}

size_t Command::getUndoSize() const
{
    return sizeof(Command);
}

bool Command::isMergeable() const
{
    return false;
}

bool Command::merge(const Command&)
{
    return false;
}

const CADacomb::GUID& Command::getMergeGuid() const
{
    return mergeGuid;
}

void Command::setMergeGuid(const CADacomb::GUID& guid)
{
    mergeGuid = guid;
}

nlohmann::json Command::createResponse(const nlohmann::json& payload) const
{
    nlohmann::json result = {
        {    "type",    getType() },
        { "version", getVersion() }
    };
    for (auto it = payload.begin(); it != payload.end(); ++it)
    {
        result[it.key()] = it.value();
    }
    return result;
}

nlohmann::json Command::createErrorResponse(const std::string& errorType, const std::string& message) const
{
    return ErrorResult(getType(), errorType, message).toJson();
}
