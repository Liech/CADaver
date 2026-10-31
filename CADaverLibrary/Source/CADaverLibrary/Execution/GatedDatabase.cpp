#include "Execution/GatedDatabase.h"

#include "CADacomb/Util/ProgressBar.h"
#include "Database/Asset/ProgressBarAsset.h"
#include "Execution/Command.h"
#include "Execution/Defer.h"
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

nlohmann::json GatedDatabase::execute(const nlohmann::json& command, std::istream* inputStream, std::ostream* outputStream)
{
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

        auto cmd = commandFactory.create(commandType);
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

        cmd->setInput(command);

        recursionLock         = true;
        nlohmann::json result = cmd->execute(database, inputStream, outputStream);
        recursionLock         = false;
        assert(result.at("type").get<std::string>() == cmd->getType());
        if (undoCallback && result.value("status", "") == "Success" && cmd->isUndoable())
        {
            undoCallback(std::make_unique<UndoContainer>(std::move(cmd)));
        }
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

void GatedDatabase::undo(UndoContainer& container)
{
    log("UNDO: " + container.getType());
    if (hasActiveTasks())
    {
        cancelAllTasks();
        while (hasActiveTasks())
        {
            Defer::getInstance().process();
            std::this_thread::yield();
        }
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
        while (hasActiveTasks())
        {
            Defer::getInstance().process();
            std::this_thread::yield();
        }
    }

    recursionLock = true;
    container.redo(database);
    recursionLock = false;
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

std::future<nlohmann::json> GatedDatabase::executeAsync(const nlohmann::json& command, std::shared_ptr<std::istream> inputStream, std::shared_ptr<std::ostream> outputStream)
{
    log("IN: " + command.dump(4));
    std::string commandType = command.value("type", "Unknown");
    auto        cmd         = commandFactory.create(commandType);
    if (!cmd)
    {
        std::promise<nlohmann::json> p;
        nlohmann::json               result = {
            { "type", commandType },
            { "version", command.value("version", 0) },
            { "status", "UnknownCommand" },
            { "error", "Unknown command type" }
        };
        log("OUT: " + result.dump(4));
        p.set_value(result);
        return p.get_future();
    }

    cmd->setInput(command);

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

    auto                        promise       = std::make_shared<std::promise<nlohmann::json>>();
    auto                        callerPromise = std::make_shared<std::promise<nlohmann::json>>();
    std::future<nlohmann::json> callerFuture  = callerPromise->get_future();

    auto cmdHolder = std::make_shared<std::unique_ptr<Command>>(std::move(cmd));

    std::thread worker(
      [this, taskGuid, progressAsset, abductedAssets, cmdHolder, inputStream, outputStream, promise, callerPromise]()
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
              result  = (*cmdHolder)->execute(isolatedDb, inputStream.get(), outputStream.get());
              success = (result.value("status", "") == "Success");
          }
          catch (const std::exception& e)
          {
              result = {
                  {    "type",    (*cmdHolder)->getType() },
                  { "version", (*cmdHolder)->getVersion() },
                  {  "status",                "Exception" },
                  {   "error",                   e.what() }
              };
          }

          if (CADacomb::ProgressBar::isCanceled(taskGuid))
          {
              result["status"] = "Canceled";
              success          = false;
          }

          std::vector<std::shared_ptr<Asset>> resultAssets;
          if (success)
          {
              resultAssets = isolatedDb.getProject().getAssets();
          }

          promise->set_value(result);
          callerPromise->set_value(result);

          Defer::getInstance().defer(
            [this, taskGuid, progressAsset, abductedAssets, resultAssets, cmdHolder, result, success]()
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

                log("OUT: " + result.dump(4));

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
