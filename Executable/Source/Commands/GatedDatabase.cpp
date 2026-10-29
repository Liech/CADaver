#include "GatedDatabase.h"

#include "CADacomb/Util/ProgressBar.h"
#include "Commands/Command.h"
#include "Commands/Defer.h"
#include "Commands/Undo/AddAssetUndoRecord.h"
#include "Commands/Undo/ReplaceAssetUndoRecord.h"
#include "Database/Asset/ProgressBarAsset.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <thread>

GatedDatabase::GatedDatabase() {}

GatedDatabase::~GatedDatabase() {}

GatedDatabase& GatedDatabase::getInstance()
{
    static GatedDatabase instance;
    return instance;
}

const Database& GatedDatabase::get() const
{
    return database;
}

void GatedDatabase::lock()
{
    if (locked)
    {
        throw std::runtime_error("GatedDatabase: Database is already locked.");
    }
    locked = true;
}

void GatedDatabase::unlock()
{
    if (!locked)
    {
        throw std::runtime_error("GatedDatabase: Database is already unlocked.");
    }
    locked = false;
}

nlohmann::json GatedDatabase::execute(const nlohmann::json& command, bool recordUndo, std::istream* inputStream, std::ostream* outputStream)
{
    if (locked)
    {
        throw std::runtime_error("GatedDatabase: Database is locked. Operations must be deferred to onLogic via Defer.");
    }

    if (recursionLock)
    {
        nlohmann::json result = {
            {    "type",                                      "Error" },
            { "command",                                           "" },
            { "version",                                            0 },
            {  "status",                                     "Locked" },
            {   "error", "Only one command can be executed at a time" }
        };
        log("OUT: " + result.dump(4));
        return result;
    }

    std::string commandType;
    int         commandVersion = 0;
    log("IN: " + command.dump(4));
    try
    {
        commandType    = command.at("type").get<std::string>();
        commandVersion = command.at("version").get<int>();

        if (command.contains("recordUndo"))
        {
            recordUndo = command.at("recordUndo").get<bool>();
        }

        auto cmd = commandFactory.create(command);
        if (!cmd)
        {
            nlohmann::json result = {
                {    "type",                                "Error" },
                { "command",                            commandType },
                { "version",                         commandVersion },
                {  "status",                       "UnknownCommand" },
                {   "error", "Unknown command type: " + commandType }
            };
            log("OUT: " + result.dump(4));
            return result;
        }

        if (hasActiveTasks() && (commandType == "Undo" || commandType == "Redo"))
        {
            cancelAllTasks();
            while (hasActiveTasks())
            {
                Defer::getInstance().process();
                std::this_thread::yield();
            }
        }

        recursionLock         = true;
        nlohmann::json result = cmd->execute(database, inputStream, outputStream, recordUndo);
        recursionLock         = false;
        assert(result.at("type").get<std::string>() == cmd->getType());
        log("OUT: " + result.dump(4));
        return result;
    }
    catch (const std::exception& e)
    {
        recursionLock         = false;
        nlohmann::json result = {
            {    "type",        "Error" },
            { "command",    commandType },
            { "version", commandVersion },
            {  "status",    "Exception" },
            {   "error",       e.what() }
        };
        log("OUT: " + result.dump(4));
        return result;
    }
}

std::string GatedDatabase::execute(const std::string& commandString, bool recordUndo, std::istream* inputStream, std::ostream* outputStream)
{
    nlohmann::json parsedCommand;
    try
    {
        parsedCommand = nlohmann::json::parse(commandString);
    }
    catch (const nlohmann::json::parse_error& e)
    {
        nlohmann::json errorResult = {
            {    "type",                                      "Error" },
            { "command",                                           "" },
            { "version",                                            0 },
            {  "status",                             "JsonParseError" },
            {   "error", std::string("JSON parse error: ") + e.what() }
        };
        return errorResult.dump();
    }

    nlohmann::json result = execute(parsedCommand, recordUndo, inputStream, outputStream);
    return result.dump();
}

void GatedDatabase::log(const std::string& msg)
{
    std::cout << msg << std::endl;
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

std::future<nlohmann::json> GatedDatabase::executeAsync(const nlohmann::json& command, bool recordUndo)
{
    if (locked)
    {
        throw std::runtime_error("GatedDatabase: Database is locked. Operations must be deferred to onLogic via Defer.");
    }

    if (command.contains("recordUndo"))
    {
        recordUndo = command.at("recordUndo").get<bool>();
    }

    auto cmd = commandFactory.create(command);
    if (!cmd)
    {
        std::promise<nlohmann::json> p;
        nlohmann::json               result = {
            { "type", command.value("type", "Unknown") },
            { "version", command.value("version", 0) },
            { "status", "UnknownCommand" },
            { "error", "Unknown command type" }
        };
        p.set_value(result);
        return p.get_future();
    }

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
    else if (command.contains("assetName") && command["assetName"].is_string())
    {
        taskName = command["assetName"].get<std::string>() + " (" + cmd->getType() + ")";
    }
    else if (command.contains("name") && command["name"].is_string())
    {
        taskName = command["name"].get<std::string>() + " (" + cmd->getType() + ")";
    }

    auto progressAsset = std::make_shared<ProgressBarAsset>(taskName, taskGuid, cmd->getType(), abductedAssets);
    database.getProject().addAsset(progressAsset);

    {
        std::lock_guard<std::mutex> lock(tasksMutex);
        activeTaskGuids.push_back(taskGuid);
    }

    auto                        promise       = std::make_shared<std::promise<nlohmann::json>>();
    auto                        callerPromise = std::make_shared<std::promise<nlohmann::json>>();
    std::future<nlohmann::json> callerFuture  = callerPromise->get_future();

    std::shared_ptr<Command> sharedCmd = std::move(cmd);

    std::thread worker(
      [this, taskGuid, progressAsset, abductedAssets, sharedCmd, recordUndo, promise, callerPromise]()
      {
          CADacomb::ProgressBar::setThreadGuid(taskGuid);

          Database isolatedDb;
          for (const auto& asset : abductedAssets)
          {
              isolatedDb.getProject().addAsset(asset);
          }

          nlohmann::json result;
          bool           success = false;

          try
          {
              result  = sharedCmd->execute(isolatedDb, nullptr, nullptr, false);
              success = (result.value("status", "") == "Success");
          }
          catch (const std::exception& e)
          {
              result = {
                  {    "type",    sharedCmd->getType() },
                  { "version", sharedCmd->getVersion() },
                  {  "status",             "Exception" },
                  {   "error",                e.what() }
              };
          }

          if (CADacomb::ProgressBar::isCanceled(taskGuid))
          {
              result["status"] = "Canceled";
              success          = false;
          }

          std::vector<std::shared_ptr<Asset>> newAssets;
          if (success)
          {
              for (const auto& asset : isolatedDb.getProject().getAssets())
              {
                  bool isOriginal = false;
                  for (const auto& orig : abductedAssets)
                  {
                      if (asset->getGuid() == orig->getGuid())
                      {
                          isOriginal = true;
                          break;
                      }
                  }
                  if (!isOriginal)
                  {
                      newAssets.push_back(asset);
                  }
              }
          }

          promise->set_value(result);
          callerPromise->set_value(result);

          Defer::getInstance().defer(
            [this, taskGuid, progressAsset, abductedAssets, newAssets, success, recordUndo]()
            {
                database.getProject().removeAsset(progressAsset->getGuid());

                if (success)
                {
                    if (newAssets.empty())
                    {
                        for (const auto& orig : abductedAssets)
                        {
                            database.getProject().addAsset(orig);
                            database.getProject().setSelectedAssetGuid(orig->getGuid());
                        }
                    }
                    else
                    {
                        for (const auto& newAsset : newAssets)
                        {
                            database.getProject().addAsset(newAsset);
                            database.getProject().setSelectedAssetGuid(newAsset->getGuid());
                            if (recordUndo)
                            {
                                if (!abductedAssets.empty())
                                {
                                    database.getUndoStack().push(std::make_unique<ReplaceAssetUndoRecord>(abductedAssets.front(), newAsset));
                                }
                                else
                                {
                                    database.getUndoStack().push(std::make_unique<AddAssetUndoRecord>(newAsset));
                                }
                            }
                        }
                    }
                }
                else
                {
                    for (const auto& orig : abductedAssets)
                    {
                        database.getProject().addAsset(orig);
                    }
                }

                {
                    std::lock_guard<std::mutex> lock(tasksMutex);
                    auto                        it = std::find(activeTaskGuids.begin(), activeTaskGuids.end(), taskGuid);
                    if (it != activeTaskGuids.end())
                    {
                        activeTaskGuids.erase(it);
                    }
                }
            });
      });

    worker.detach();

    std::shared_future<nlohmann::json> sharedFut = promise->get_future().share();
    progressAsset->setFuture(sharedFut);

    return callerFuture;
}

std::future<std::string> GatedDatabase::executeAsync(const std::string& commandString, bool recordUndo)
{
    nlohmann::json parsedCommand;
    try
    {
        parsedCommand = nlohmann::json::parse(commandString);
    }
    catch (const nlohmann::json::parse_error& e)
    {
        std::promise<std::string> p;
        nlohmann::json            err = {
            {    "type",                                      "Error" },
            { "command",                                           "" },
            { "version",                                            0 },
            {  "status",                             "JsonParseError" },
            {   "error", std::string("JSON parse error: ") + e.what() }
        };
        p.set_value(err.dump());
        return p.get_future();
    }

    auto fut = executeAsync(parsedCommand, recordUndo);
    return std::async(std::launch::deferred, [f = std::move(fut)]() mutable { return f.get().dump(); });
}
