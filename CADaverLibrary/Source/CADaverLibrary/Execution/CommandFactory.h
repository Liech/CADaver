#pragma once

#include "Execution/Command.h"

#include <functional>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>
#include <vector>

// Factory responsible for parsing JSON commands and instantiating registered Command objects.
class CommandFactory
{
  public:
    CommandFactory();
    virtual ~CommandFactory();

    std::unique_ptr<Command>              create(const nlohmann::json& commandJson) const;
    std::vector<std::unique_ptr<Command>> getAllCommands() const;

    void registerCommand(const std::string& name, std::function<std::unique_ptr<Command>(const nlohmann::json&)> creator, std::function<std::unique_ptr<Command>()> defaultCreator = nullptr);

  private:
    std::unordered_map<std::string, std::function<std::unique_ptr<Command>(const nlohmann::json&)>> creators;
    std::unordered_map<std::string, std::function<std::unique_ptr<Command>()>>                      defaultCreators;
    std::vector<std::string>                                                                        registeredOrder;
};
