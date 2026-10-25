#pragma once

#include <istream>
#include <nlohmann/json.hpp>
#include <ostream>
#include <string>

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

    virtual nlohmann::json execute(Database& database, std::istream* inputStream, std::ostream* outputStream) = 0;
};
