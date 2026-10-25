#include "CommandFactory.h"

#include "ClearProjectCommand.h"
#include "LoadStepAssetCommand.h"
#include "LoadStlAssetCommand.h"
#include "LoadSvoAssetCommand.h"
#include "RemoveAssetCommand.h"
#include "SaveStepAssetCommand.h"
#include "SaveStlAssetCommand.h"
#include "SaveVoxelAssetCommand.h"
#include "UpdateCameraPreferencesCommand.h"
#include "UpdateRenderPreferencesCommand.h"
#include "UpdateSVOPreferencesCommand.h"
#include "UpdateUIPreferencesCommand.h"

CommandFactory::CommandFactory()
{
    registerCommands();
}

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
    commands.push_back(std::make_unique<ClearProjectCommand>());
    commands.push_back(std::make_unique<LoadStepAssetCommand>());
    commands.push_back(std::make_unique<LoadStlAssetCommand>());
    commands.push_back(std::make_unique<LoadSvoAssetCommand>());
    commands.push_back(std::make_unique<RemoveAssetCommand>());
    commands.push_back(std::make_unique<SaveStepAssetCommand>());
    commands.push_back(std::make_unique<SaveStlAssetCommand>());
    commands.push_back(std::make_unique<SaveVoxelAssetCommand>());
    commands.push_back(std::make_unique<UpdateCameraPreferencesCommand>());
    commands.push_back(std::make_unique<UpdateRenderPreferencesCommand>());
    commands.push_back(std::make_unique<UpdateSVOPreferencesCommand>());
    commands.push_back(std::make_unique<UpdateUIPreferencesCommand>());
    return commands;
}

void CommandFactory::registerCommand(const std::string& name, std::function<std::unique_ptr<Command>(const nlohmann::json&)> creator)
{
    creators[name] = std::move(creator);
}

void CommandFactory::registerCommands()
{
    registerCommand("RemoveAsset", [](const nlohmann::json& json) { return std::make_unique<RemoveAssetCommand>(json); });
    registerCommand("ClearProject", [](const nlohmann::json& json) { return std::make_unique<ClearProjectCommand>(json); });
    registerCommand("LoadStlAsset", [](const nlohmann::json& json) { return std::make_unique<LoadStlAssetCommand>(json); });
    registerCommand("SaveStlAsset", [](const nlohmann::json& json) { return std::make_unique<SaveStlAssetCommand>(json); });
    registerCommand("LoadStepAsset", [](const nlohmann::json& json) { return std::make_unique<LoadStepAssetCommand>(json); });
    registerCommand("SaveStepAsset", [](const nlohmann::json& json) { return std::make_unique<SaveStepAssetCommand>(json); });
    registerCommand("LoadSvoAsset", [](const nlohmann::json& json) { return std::make_unique<LoadSvoAssetCommand>(json); });
    registerCommand("SaveVoxelAsset", [](const nlohmann::json& json) { return std::make_unique<SaveVoxelAssetCommand>(json); });
    registerCommand("UpdateCameraPreferences", [](const nlohmann::json& json) { return std::make_unique<UpdateCameraPreferencesCommand>(json); });
    registerCommand("UpdateUIPreferences", [](const nlohmann::json& json) { return std::make_unique<UpdateUIPreferencesCommand>(json); });
    registerCommand("UpdateRenderPreferences", [](const nlohmann::json& json) { return std::make_unique<UpdateRenderPreferencesCommand>(json); });
    registerCommand("UpdateSVOPreferences", [](const nlohmann::json& json) { return std::make_unique<UpdateSVOPreferencesCommand>(json); });
}
