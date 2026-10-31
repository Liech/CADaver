#pragma once

#include "CADacomb/Util/GUID.h"

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

    virtual nlohmann::json execute(Database& database, std::istream* inputStream, std::ostream* outputStream, bool recordUndo = false) = 0;

  protected:
    nlohmann::json createResponse(const std::string& status, const nlohmann::json& payload = nlohmann::json::object()) const;
};
