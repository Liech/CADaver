#pragma once

#include "Execution/Command.h"

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

// Factory responsible for instantiating registered Command objects by type name.
class CommandFactory
{
  public:
    CommandFactory();
    virtual ~CommandFactory();

    std::unique_ptr<Command>              create(const std::string& type) const;
    std::vector<std::unique_ptr<Command>> getAllCommands() const;

    void registerCommand(const std::string& name, std::function<std::unique_ptr<Command>()> creator);

  private:
    std::unordered_map<std::string, std::function<std::unique_ptr<Command>()>> creators;
    std::vector<std::string>                                                   registeredOrder;
};
