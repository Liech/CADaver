#include "Execution/CommandFactory.h"

#include <algorithm>

CommandFactory::CommandFactory() {}

CommandFactory::~CommandFactory() {}

std::unique_ptr<Command> CommandFactory::create(const std::string& type) const
{
    auto it = creators.find(type);
    if (it != creators.end())
    {
        return it->second();
    }

    return nullptr;
}

std::vector<std::unique_ptr<Command>> CommandFactory::getAllCommands() const
{
    std::vector<std::unique_ptr<Command>> commands;
    for (const auto& name : registeredOrder)
    {
        auto it = creators.find(name);
        if (it != creators.end() && it->second)
        {
            auto cmd = it->second();
            if (cmd)
            {
                commands.push_back(std::move(cmd));
            }
        }
    }
    return commands;
}

void CommandFactory::registerCommand(const std::string& name, std::function<std::unique_ptr<Command>()> creator)
{
    if (std::find(registeredOrder.begin(), registeredOrder.end(), name) == registeredOrder.end())
    {
        registeredOrder.push_back(name);
    }
    creators[name] = std::move(creator);
}
