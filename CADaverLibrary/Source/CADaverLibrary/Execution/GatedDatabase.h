#pragma once

#include "CADacomb/Util/GUID.h"
#include "Database/Database.h"
#include "Execution/CommandFactory.h"
#include "Polyglot/Node.h"

#include <functional>
#include <future>
#include <istream>
#include <memory>
#include <mutex>
#include <nlohmann/json.hpp>
#include <ostream>
#include <string>
#include <vector>

class UndoContainer;

// Guards access to the Database with a JSON and stream-based command boundary.
class GatedDatabase
{
  public:
    GatedDatabase();
    virtual ~GatedDatabase();

    const Database&       get() const;
    CommandFactory&       getCommandFactory();
    const CommandFactory& getCommandFactory() const;

    Polyglot::Node              execute(const Polyglot::Node& command);
    std::future<Polyglot::Node> executeAsync(const Polyglot::Node& command);

    void undo(UndoContainer& container);
    void redo(UndoContainer& container);

    bool hasActiveTasks() const;
    void cancelAllTasks();

    void setUndoCallback(std::function<void(std::unique_ptr<UndoContainer>)> callback);
    void setDispatchCallback(std::function<void(std::function<void()>)> callback);
    void setLogCallback(std::function<void(const std::string&)> callback);

  private:
    void log(const std::string& msg);

    Database                                            database;
    CommandFactory                                      commandFactory;
    std::function<void(std::unique_ptr<UndoContainer>)> undoCallback;
    std::function<void(std::function<void()>)>          dispatchCallback;
    std::function<void(const std::string&)>             logCallback;
    mutable std::mutex                                  tasksMutex;
    std::vector<CADacomb::GUID>                         activeTaskGuids;
    bool                                                recursionLock = false;
};
