#include "CommandFactory.h"

#include "LoadStepAssetCommand.h"
#include "LoadStlAssetCommand.h"
#include "LoadSvoAssetCommand.h"
#include "SaveVoxelAssetCommand.h"
#include "SelectAssetCommand.h"
#include "UpdateCameraPreferencesCommand.h"
#include "UpdateRenderPreferencesCommand.h"
#include "UpdateUIPreferencesCommand.h"

CommandFactory::CommandFactory()
{
    registerCommands();
}

CommandFactory::~CommandFactory() {}

std::unique_ptr<Command> CommandFactory::create(const nlohmann::json& commandJson) const
{
    std::string commandType;
    if (commandJson.contains("type"))
    {
        commandType = commandJson.at("type").get<std::string>();
    }
    else
    {
        commandType = commandJson.at("command").get<std::string>();
    }

    auto it = creators.find(commandType);
    if (it != creators.end())
    {
        return it->second(commandJson);
    }

    return nullptr;
}

void CommandFactory::registerCommand(const std::string& name, std::function<std::unique_ptr<Command>(const nlohmann::json&)> creator)
{
    creators[name] = std::move(creator);
}

void CommandFactory::registerCommands()
{
    registerCommand("SelectAsset", [](const nlohmann::json& json) { return std::make_unique<SelectAssetCommand>(json); });
    registerCommand("LoadStlAsset", [](const nlohmann::json& json) { return std::make_unique<LoadStlAssetCommand>(json); });
    registerCommand("LoadStepAsset", [](const nlohmann::json& json) { return std::make_unique<LoadStepAssetCommand>(json); });
    registerCommand("LoadSvoAsset", [](const nlohmann::json& json) { return std::make_unique<LoadSvoAssetCommand>(json); });
    registerCommand("SaveVoxelAsset", [](const nlohmann::json& json) { return std::make_unique<SaveVoxelAssetCommand>(json); });
    registerCommand("UpdateCameraPreferences", [](const nlohmann::json& json) { return std::make_unique<UpdateCameraPreferencesCommand>(json); });
    registerCommand("UpdateUIPreferences", [](const nlohmann::json& json) { return std::make_unique<UpdateUIPreferencesCommand>(json); });
    registerCommand("UpdateRenderPreferences", [](const nlohmann::json& json) { return std::make_unique<UpdateRenderPreferencesCommand>(json); });
}
