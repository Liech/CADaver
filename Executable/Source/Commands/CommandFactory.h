#pragma once

#include "Command.h"

#include <functional>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>

// Factory responsible for parsing JSON commands and instantiating concrete Command objects.
class CommandFactory
{
  public:
    CommandFactory();
    virtual ~CommandFactory();

    std::unique_ptr<Command> create(const nlohmann::json& commandJson) const;

    void registerCommand(const std::string& name, std::function<std::unique_ptr<Command>(const nlohmann::json&)> creator);

  private:
    void registerCommands();

    std::unordered_map<std::string, std::function<std::unique_ptr<Command>(const nlohmann::json&)>> creators;
};
