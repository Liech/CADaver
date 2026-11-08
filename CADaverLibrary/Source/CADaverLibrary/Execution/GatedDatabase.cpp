#include "Execution/GatedDatabase.h"

#include "CADacomb/Util/ProgressBar.h"
#include "Database/Asset/ProgressBarAsset.h"
#include "Execution/Command.h"
#include "Execution/UndoContainer.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <thread>

GatedDatabase::GatedDatabase() {}

GatedDatabase::~GatedDatabase() {}

const Database& GatedDatabase::get() const
{
    return database;
}

CommandFactory& GatedDatabase::getCommandFactory()
{
    return commandFactory;
}

const CommandFactory& GatedDatabase::getCommandFactory() const
{
    return commandFactory;
}

void GatedDatabase::setUndoCallback(std::function<void(std::unique_ptr<UndoContainer>)> callback)
{
    undoCallback = std::move(callback);
}

void GatedDatabase::setDispatchCallback(std::function<void(std::function<void()>)> callback)
{
    dispatchCallback = std::move(callback);
}

void GatedDatabase::setLogCallback(std::function<void(const std::string&)> callback)
{
    logCallback = std::move(callback);
}

Polyglot::Node GatedDatabase::execute(const Polyglot::Node& command)
{
    Polyglot::Node commandNode = command;
    if (recursionLock)
    {
        Polyglot::Node result = {
            {    "type",                                      "Error" },
            { "command",                                           "" },
            { "version",                                            0 },
            {  "status",                                     "Locked" },
            {   "error", "Only one command can be executed at a time" }
        };
        log("OUT: " + result.toJson().dump(4));
        return result;
    }

    std::string commandType;
    int         commandVersion = 0;
    log("IN: " + commandNode.toJson().dump(4));
    try
    {
        commandType    = static_cast<std::string>(commandNode["type"]);
        commandVersion = static_cast<int>(commandNode["version"]);

        auto cmd = commandFactory.create(commandType);
        if (!cmd)
        {
            Polyglot::Node result = {
                {    "type",                                "Error" },
                { "command",                            commandType },
                { "version",                         commandVersion },
                {  "status",                       "UnknownCommand" },
                {   "error", "Unknown command type: " + commandType }
            };
            log("OUT: " + result.toJson().dump(4));
            return result;
        }

        cmd->setInput(commandNode);

        recursionLock         = true;
        nlohmann::json result = cmd->execute(database);
        recursionLock         = false;
        assert(result.at("type").get<std::string>() == cmd->getType());
        if (undoCallback && result.value("status", "") == "Success" && cmd->isUndoable())
        {
            undoCallback(std::make_unique<UndoContainer>(std::move(cmd)));
        }
        log("OUT: " + result.dump(4));
        return Polyglot::Node(result);
    }
    catch (const std::exception& e)
    {
        recursionLock         = false;
        Polyglot::Node result = {
            {    "type",        "Error" },
            { "command",    commandType },
            { "version", commandVersion },
            {  "status",    "Exception" },
            {   "error",       e.what() }
        };
        log("OUT: " + result.toJson().dump(4));
        return result;
    }
}

void GatedDatabase::undo(UndoContainer& container)
{
    log("UNDO: " + container.getType());
    if (hasActiveTasks())
    {
        cancelAllTasks();
    }

    recursionLock = true;
    container.undo(database);
    recursionLock = false;
}

void GatedDatabase::redo(UndoContainer& container)
{
    log("REDO: " + container.getType());
    if (hasActiveTasks())
    {
        cancelAllTasks();
    }

    recursionLock = true;
    container.redo(database);
    recursionLock = false;
}

void GatedDatabase::log(const std::string& msg)
{
    if (logCallback)
    {
        logCallback(msg);
    }
    else
    {
        std::cout << msg << std::endl;
    }
}

bool GatedDatabase::hasActiveTasks() const
{
    std::lock_guard<std::mutex> lock(tasksMutex);
    return !activeTaskGuids.empty();
}

void GatedDatabase::cancelAllTasks()
{
    std::lock_guard<std::mutex> lock(tasksMutex);
    for (const auto& guid : activeTaskGuids)
    {
        CADacomb::ProgressBar::cancel(guid);
    }
}

std::future<Polyglot::Node> GatedDatabase::executeAsync(const Polyglot::Node& command)
{
    Polyglot::Node commandNode = command;
    log("IN: " + commandNode.toJson().dump(4));
    std::string commandType    = static_cast<std::string>(commandNode["type"]);
    int         commandVersion = static_cast<int>(commandNode["version"]);

    auto cmd = commandFactory.create(commandType);
    if (!cmd)
    {
        std::promise<Polyglot::Node> p;
        Polyglot::Node               result = {
            {    "type",                            commandType },
            { "version",                         commandVersion },
            {  "status",                       "UnknownCommand" },
            {   "error", "Unknown command type: " + commandType }
        };
        log("OUT: " + result.toJson().dump(4));
        p.set_value(result);
        return p.get_future();
    }

    cmd->setInput(commandNode);

    auto                                requiredGuids = cmd->getRequiredAssets();
    std::vector<std::shared_ptr<Asset>> abductedAssets;
    for (const auto& guid : requiredGuids)
    {
        auto asset = database.getProject().getAsset(guid);
        if (asset)
        {
            abductedAssets.push_back(asset);
        }
    }

    for (const auto& asset : abductedAssets)
    {
        database.getProject().removeAsset(asset->getGuid());
    }

    CADacomb::GUID taskGuid = CADacomb::GUID::random();
    std::string    taskName = cmd->getType();
    if (!abductedAssets.empty())
    {
        taskName = abductedAssets.front()->getName() + " (" + cmd->getType() + ")";
    }

    auto progressAsset = std::make_shared<ProgressBarAsset>();
    progressAsset->setName(taskName);
    progressAsset->setTaskGuid(taskGuid);
    progressAsset->setOperationName(cmd->getType());
    progressAsset->setAbductedAssets(abductedAssets);
    database.getProject().addAsset(progressAsset);

    {
        std::lock_guard<std::mutex> lock(tasksMutex);
        activeTaskGuids.push_back(taskGuid);
    }

    auto                               callerPromise   = std::make_shared<std::promise<Polyglot::Node>>();
    auto                               progressPromise = std::make_shared<std::promise<Polyglot::Node>>();
    std::future<Polyglot::Node>        callerFuture    = callerPromise->get_future();
    std::shared_future<Polyglot::Node> sharedFuture    = progressPromise->get_future().share();
    progressAsset->setFuture(sharedFuture);

    auto cmdHolder = std::make_shared<std::unique_ptr<Command>>(std::move(cmd));

    std::thread worker(
      [this, taskGuid, progressAsset, abductedAssets, cmdHolder, callerPromise, progressPromise]()
      {
          CADacomb::ProgressBar::setThreadGuid(taskGuid);

          Database isolatedDb;
          for (const auto& asset : abductedAssets)
          {
              isolatedDb.getProject().addAsset(asset);
          }

          nlohmann::json executionResult;
          bool           success = false;

          try
          {
              executionResult = (*cmdHolder)->execute(isolatedDb);
              success         = (executionResult.value("status", "") == "Success");
          }
          catch (const std::exception& e)
          {
              executionResult = {
                  {    "type",    (*cmdHolder)->getType() },
                  { "version", (*cmdHolder)->getVersion() },
                  {  "status",                "Exception" },
                  {   "error",                   e.what() }
              };
          }

          if (CADacomb::ProgressBar::isCanceled(taskGuid))
          {
              executionResult["status"] = "Canceled";
              success                   = false;
          }

          std::vector<std::shared_ptr<Asset>> resultAssets;
          if (success)
          {
              resultAssets = isolatedDb.getProject().getAssets();
          }

          Polyglot::Node resultNode(executionResult);
          callerPromise->set_value(resultNode);
          progressPromise->set_value(resultNode);

          auto completion = [this, taskGuid, progressAsset, abductedAssets, resultAssets, cmdHolder, executionResult, success]()
          {
              database.getProject().removeAsset(progressAsset->getGuid());

              if (success)
              {
                  for (const auto& asset : resultAssets)
                  {
                      database.getProject().addAsset(asset);
                  }
                  if (undoCallback && *cmdHolder && (*cmdHolder)->isUndoable())
                  {
                      undoCallback(std::make_unique<UndoContainer>(std::move(*cmdHolder)));
                  }
              }
              else
              {
                  for (const auto& orig : abductedAssets)
                  {
                      database.getProject().addAsset(orig);
                  }
              }

              log("OUT: " + executionResult.dump(4));

              {
                  std::lock_guard<std::mutex> lock(tasksMutex);
                  auto                        it = std::find(activeTaskGuids.begin(), activeTaskGuids.end(), taskGuid);
                  if (it != activeTaskGuids.end())
                  {
                      activeTaskGuids.erase(it);
                  }
              }
          };

          if (dispatchCallback)
          {
              dispatchCallback(completion);
          }
          else
          {
              completion();
          }
      });

    worker.detach();

    return callerFuture;
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("GatedDatabase/LogCallback")
{
    GatedDatabase            gatedDatabase;
    std::vector<std::string> logs;
    gatedDatabase.setLogCallback([&logs](const std::string& msg) { logs.push_back(msg); });

    gatedDatabase.execute({
      {    "type", "ClearProject" },
      { "version",              1 }
    });

    CHECK(logs.size() >= 2);
}
#endif
