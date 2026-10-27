#include "Commands/CommandFactory.h"

#include "Commands/Converter/ConvertCadToStlCommand.h"
#include "Commands/Converter/ConvertCadToSvoCommand.h"
#include "Commands/Converter/ConvertStlToSvoCommand.h"
#include "Commands/Converter/ConvertSvoToStlCommand.h"
#include "Commands/IO/LoadStepAssetCommand.h"
#include "Commands/IO/LoadStlAssetCommand.h"
#include "Commands/IO/LoadSvoAssetCommand.h"
#include "Commands/IO/SaveStepAssetCommand.h"
#include "Commands/IO/SaveStlAssetCommand.h"
#include "Commands/IO/SaveVoxelAssetCommand.h"
#include "Commands/Preferences/UpdateCameraPreferencesCommand.h"
#include "Commands/Preferences/UpdateRenderPreferencesCommand.h"
#include "Commands/Preferences/UpdateSVOPreferencesCommand.h"
#include "Commands/Preferences/UpdateUIPreferencesCommand.h"
#include "Commands/Project/ClearProjectCommand.h"
#include "Commands/Project/RemoveAssetCommand.h"
#include "Commands/Undo/RedoCommand.h"
#include "Commands/Undo/UndoCommand.h"

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
    commands.push_back(std::make_unique<ConvertCadToStlCommand>());
    commands.push_back(std::make_unique<ConvertCadToSvoCommand>());
    commands.push_back(std::make_unique<ConvertStlToSvoCommand>());
    commands.push_back(std::make_unique<ConvertSvoToStlCommand>());
    commands.push_back(std::make_unique<LoadStepAssetCommand>());
    commands.push_back(std::make_unique<LoadStlAssetCommand>());
    commands.push_back(std::make_unique<LoadSvoAssetCommand>());
    commands.push_back(std::make_unique<RemoveAssetCommand>());
    commands.push_back(std::make_unique<SaveStepAssetCommand>());
    commands.push_back(std::make_unique<SaveStlAssetCommand>());
    commands.push_back(std::make_unique<SaveVoxelAssetCommand>());
    commands.push_back(std::make_unique<UndoCommand>());
    commands.push_back(std::make_unique<RedoCommand>());
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
    registerCommand("ConvertCadToStl", [](const nlohmann::json& json) { return std::make_unique<ConvertCadToStlCommand>(json); });
    registerCommand("ConvertCadToSvo", [](const nlohmann::json& json) { return std::make_unique<ConvertCadToSvoCommand>(json); });
    registerCommand("ConvertStlToSvo", [](const nlohmann::json& json) { return std::make_unique<ConvertStlToSvoCommand>(json); });
    registerCommand("ConvertSvoToStl", [](const nlohmann::json& json) { return std::make_unique<ConvertSvoToStlCommand>(json); });
    registerCommand("LoadStlAsset", [](const nlohmann::json& json) { return std::make_unique<LoadStlAssetCommand>(json); });
    registerCommand("SaveStlAsset", [](const nlohmann::json& json) { return std::make_unique<SaveStlAssetCommand>(json); });
    registerCommand("LoadStepAsset", [](const nlohmann::json& json) { return std::make_unique<LoadStepAssetCommand>(json); });
    registerCommand("SaveStepAsset", [](const nlohmann::json& json) { return std::make_unique<SaveStepAssetCommand>(json); });
    registerCommand("LoadSvoAsset", [](const nlohmann::json& json) { return std::make_unique<LoadSvoAssetCommand>(json); });
    registerCommand("SaveVoxelAsset", [](const nlohmann::json& json) { return std::make_unique<SaveVoxelAssetCommand>(json); });
    registerCommand("Undo", [](const nlohmann::json& json) { return std::make_unique<UndoCommand>(json); });
    registerCommand("Redo", [](const nlohmann::json& json) { return std::make_unique<RedoCommand>(json); });
    registerCommand("UpdateCameraPreferences", [](const nlohmann::json& json) { return std::make_unique<UpdateCameraPreferencesCommand>(json); });
    registerCommand("UpdateUIPreferences", [](const nlohmann::json& json) { return std::make_unique<UpdateUIPreferencesCommand>(json); });
    registerCommand("UpdateRenderPreferences", [](const nlohmann::json& json) { return std::make_unique<UpdateRenderPreferencesCommand>(json); });
    registerCommand("UpdateSVOPreferences", [](const nlohmann::json& json) { return std::make_unique<UpdateSVOPreferencesCommand>(json); });
}
