#pragma once

#include "CADacomb/Util/GUID.h"

#include <cstddef>
#include <istream>
#include <nlohmann/json.hpp>
#include <ostream>
#include <string>
#include <vector>

class Database;

// Abstract base class for all gated database commands.
class Command
{
  public:
    Command();
    virtual ~Command();

    virtual std::string getType() const        = 0;
    virtual int         getVersion() const     = 0;
    virtual std::string getDescription() const = 0;

    virtual std::vector<CADacomb::GUID> getRequiredAssets() const;

    virtual void           setInput(const nlohmann::json& json);
    virtual nlohmann::json execute(Database& database, std::istream* inputStream = nullptr, std::ostream* outputStream = nullptr) = 0;

    virtual bool   isUndoable() const;
    virtual void   undo(Database& database);
    virtual void   redo(Database& database);
    virtual size_t getUndoSize() const;

  protected:
    nlohmann::json createResponse(const std::string& status, const nlohmann::json& payload = nlohmann::json::object()) const;
};
