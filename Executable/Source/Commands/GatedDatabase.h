#pragma once

#include "CommandFactory.h"
#include "Database/Database.h"

#include <istream>
#include <memory>
#include <nlohmann/json.hpp>
#include <ostream>
#include <string>

// Guards access to the Database with a string and stream-based command boundary.
class GatedDatabase
{
  public:
    GatedDatabase();
    virtual ~GatedDatabase();

    static GatedDatabase& getInstance();

    const Database& get() const;

    void lock();
    void unlock();

    nlohmann::json execute(const nlohmann::json& command, std::istream* inputStream = nullptr, std::ostream* outputStream = nullptr);
    std::string    execute(const std::string& commandString, std::istream* inputStream = nullptr, std::ostream* outputStream = nullptr);

  private:
    void log(const std::string& msg);

    Database       database;
    CommandFactory commandFactory;
    bool           locked        = true;
    bool           recursionLock = false;
};
