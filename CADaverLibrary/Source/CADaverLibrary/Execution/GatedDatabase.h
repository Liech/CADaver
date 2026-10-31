#pragma once

#include "CADacomb/Util/GUID.h"
#include "Database/Database.h"
#include "Execution/CommandFactory.h"

#include <future>
#include <istream>
#include <memory>
#include <mutex>
#include <nlohmann/json.hpp>
#include <ostream>
#include <string>
#include <vector>

// Guards access to the Database with a JSON and stream-based command boundary.
class GatedDatabase
{
  public:
    GatedDatabase();
    virtual ~GatedDatabase();

    const Database&       get() const;
    CommandFactory&       getCommandFactory();
    const CommandFactory& getCommandFactory() const;

    nlohmann::json              execute(const nlohmann::json& command, std::istream* inputStream = nullptr, std::ostream* outputStream = nullptr);
    std::future<nlohmann::json> executeAsync(const nlohmann::json& command, std::shared_ptr<std::istream> inputStream = nullptr, std::shared_ptr<std::ostream> outputStream = nullptr);

    bool hasActiveTasks() const;
    void cancelAllTasks();

  private:
    void log(const std::string& msg);

    Database                    database;
    CommandFactory              commandFactory;
    mutable std::mutex          tasksMutex;
    std::vector<CADacomb::GUID> activeTaskGuids;
    bool                        recursionLock = false;
};
