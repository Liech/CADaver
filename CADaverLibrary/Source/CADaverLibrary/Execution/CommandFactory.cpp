#include "Execution/CommandFactory.h"

#include <algorithm>

CommandFactory::CommandFactory() {}

CommandFactory::~CommandFactory() {}

std::unique_ptr<Command> CommandFactory::create(const nlohmann::json& commandJson) const
{
    std::string commandType = commandJson.at("type").get<std::string>();

    auto it = creators.find(commandType);
    if (it != creators.end())
    {
        return it->second(commandJson);
    }

    return nullptr;
}

std::vector<std::unique_ptr<Command>> CommandFactory::getAllCommands() const
{
    std::vector<std::unique_ptr<Command>> commands;
    for (const auto& name : registeredOrder)
    {
        auto defIt = defaultCreators.find(name);
        if (defIt != defaultCreators.end() && defIt->second)
        {
            auto cmd = defIt->second();
            if (cmd)
            {
                commands.push_back(std::move(cmd));
                continue;
            }
        }

        auto it = creators.find(name);
        if (it != creators.end() && it->second)
        {
            try
            {
                auto cmd = it->second(nlohmann::json::object());
                if (cmd)
                {
                    commands.push_back(std::move(cmd));
                }
            }
            catch (...)
            {
            }
        }
    }
    return commands;
}

void CommandFactory::registerCommand(const std::string& name, std::function<std::unique_ptr<Command>(const nlohmann::json&)> creator, std::function<std::unique_ptr<Command>()> defaultCreator)
{
    if (std::find(registeredOrder.begin(), registeredOrder.end(), name) == registeredOrder.end())
    {
        registeredOrder.push_back(name);
    }
    creators[name]        = std::move(creator);
    defaultCreators[name] = std::move(defaultCreator);
}
