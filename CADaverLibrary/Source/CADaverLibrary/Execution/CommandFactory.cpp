#include "Execution/CommandFactory.h"

#include "Commands/Component/AddNoteCommand.h"
#include "Commands/Component/GetNotesCommand.h"
#include "Commands/Component/RemoveComponentCommand.h"
#include "Commands/Component/UpdateNoteCommand.h"
#include "Commands/Conversion/ConvertCadToStlCommand.h"
#include "Commands/Conversion/ConvertCadToSvoCommand.h"
#include "Commands/Conversion/ConvertStlToSvoCommand.h"
#include "Commands/Conversion/ConvertSvoToStlCommand.h"
#include "Commands/Generator/GenerateLandscapeCommand.h"
#include "Commands/Geometry/GetAssetAABBCommand.h"
#include "Commands/Geometry/GetProjectAABBCommand.h"
#include "Commands/IO/LoadStepAssetCommand.h"
#include "Commands/IO/LoadStlAssetCommand.h"
#include "Commands/IO/LoadSvoAssetCommand.h"
#include "Commands/IO/SaveStepAssetCommand.h"
#include "Commands/IO/SaveStlAssetCommand.h"
#include "Commands/IO/SaveVoxelAssetCommand.h"
#include "Commands/Preferences/Query/GetCameraPreferencesCommand.h"
#include "Commands/Preferences/Query/GetRenderPreferencesCommand.h"
#include "Commands/Preferences/Query/GetSVOPreferencesCommand.h"
#include "Commands/Preferences/Query/GetUIPreferencesCommand.h"
#include "Commands/Preferences/UpdateCameraPreferencesCommand.h"
#include "Commands/Preferences/UpdateRenderPreferencesCommand.h"
#include "Commands/Preferences/UpdateSVOPreferencesCommand.h"
#include "Commands/Preferences/UpdateUIPreferencesCommand.h"
#include "Commands/Project/ClearProjectCommand.h"
#include "Commands/Project/GetAssetCountCommand.h"
#include "Commands/Project/GetAssetInfoCommand.h"
#include "Commands/Project/GetAssetNamesCommand.h"
#include "Commands/Project/GetAssetsCommand.h"
#include "Commands/Project/RemoveAssetCommand.h"
#include "Commands/Search/GetDocumentationCommand.h"
#include "Commands/Search/SearchCommand.h"
#include "Commands/Task/CancelTaskCommand.h"
#include "Commands/Task/GetActiveTasksCommand.h"
#include "Commands/Task/GetTaskStateCommand.h"

#include <algorithm>

CommandFactory::CommandFactory()
{
    registerCommands();
}

CommandFactory::~CommandFactory() {}

std::unique_ptr<Command> CommandFactory::create(const std::string& type) const
{
    auto it = creators.find(type);
    if (it != creators.end() && it->second)
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

void CommandFactory::registerCommands()
{
    registerCommand("AddNote", []() { return std::make_unique<AddNoteCommand>(); });
    registerCommand("GetNotes", []() { return std::make_unique<GetNotesCommand>(); });
    registerCommand("UpdateNote", []() { return std::make_unique<UpdateNoteCommand>(); });
    registerCommand("RemoveComponent", []() { return std::make_unique<RemoveComponentCommand>(); });
    registerCommand("RemoveAsset", []() { return std::make_unique<RemoveAssetCommand>(); });
    registerCommand("ClearProject", []() { return std::make_unique<ClearProjectCommand>(); });
    registerCommand("ConvertCadToStl", []() { return std::make_unique<ConvertCadToStlCommand>(); });
    registerCommand("ConvertCadToSvo", []() { return std::make_unique<ConvertCadToSvoCommand>(); });
    registerCommand("ConvertStlToSvo", []() { return std::make_unique<ConvertStlToSvoCommand>(); });
    registerCommand("ConvertSvoToStl", []() { return std::make_unique<ConvertSvoToStlCommand>(); });
    registerCommand("GenerateLandscape", []() { return std::make_unique<GenerateLandscapeCommand>(); });
    registerCommand("LoadStlAsset", []() { return std::make_unique<LoadStlAssetCommand>(); });
    registerCommand("SaveStlAsset", []() { return std::make_unique<SaveStlAssetCommand>(); });
    registerCommand("LoadStepAsset", []() { return std::make_unique<LoadStepAssetCommand>(); });
    registerCommand("SaveStepAsset", []() { return std::make_unique<SaveStepAssetCommand>(); });
    registerCommand("LoadSvoAsset", []() { return std::make_unique<LoadSvoAssetCommand>(); });
    registerCommand("SaveVoxelAsset", []() { return std::make_unique<SaveVoxelAssetCommand>(); });
    registerCommand("UpdateCameraPreferences", []() { return std::make_unique<UpdateCameraPreferencesCommand>(); });
    registerCommand("UpdateUIPreferences", []() { return std::make_unique<UpdateUIPreferencesCommand>(); });
    registerCommand("UpdateRenderPreferences", []() { return std::make_unique<UpdateRenderPreferencesCommand>(); });
    registerCommand("UpdateSVOPreferences", []() { return std::make_unique<UpdateSVOPreferencesCommand>(); });
    registerCommand("GetAssetCount", []() { return std::make_unique<GetAssetCountCommand>(); });
    registerCommand("GetAssetInfo", []() { return std::make_unique<GetAssetInfoCommand>(); });
    registerCommand("GetAssetNames", []() { return std::make_unique<GetAssetNamesCommand>(); });
    registerCommand("GetAssets", []() { return std::make_unique<GetAssetsCommand>(); });
    registerCommand("GetCameraPreferences", []() { return std::make_unique<GetCameraPreferencesCommand>(); });
    registerCommand("GetRenderPreferences", []() { return std::make_unique<GetRenderPreferencesCommand>(); });
    registerCommand("GetSVOPreferences", []() { return std::make_unique<GetSVOPreferencesCommand>(); });
    registerCommand("GetUIPreferences", []() { return std::make_unique<GetUIPreferencesCommand>(); });
    registerCommand("GetActiveTasks", []() { return std::make_unique<GetActiveTasksCommand>(); });
    registerCommand("GetTaskState", []() { return std::make_unique<GetTaskStateCommand>(); });
    registerCommand("CancelTask", []() { return std::make_unique<CancelTaskCommand>(); });
    registerCommand("GetAssetAABB", []() { return std::make_unique<GetAssetAABBCommand>(); });
    registerCommand("GetProjectAABB", []() { return std::make_unique<GetProjectAABBCommand>(); });
    registerCommand("Search", []() { return std::make_unique<SearchCommand>(); });
    registerCommand("GetDocumentation", []() { return std::make_unique<GetDocumentationCommand>(); });
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("CommandFactory/DefaultRegisteredCommands")
{
    CommandFactory factory;
    CHECK(factory.create("NonExistent") == nullptr);
    CHECK(factory.create("RemoveAsset") != nullptr);
    CHECK(factory.create("ClearProject") != nullptr);
    CHECK(factory.create("GetAssetCount") != nullptr);
    CHECK(factory.create("GetAssetInfo") != nullptr);
    CHECK(factory.create("GetAssetNames") != nullptr);
    CHECK(factory.create("GetAssets") != nullptr);
    CHECK(factory.create("GetCameraPreferences") != nullptr);
    CHECK(factory.create("GetRenderPreferences") != nullptr);
    CHECK(factory.create("GetSVOPreferences") != nullptr);
    CHECK(factory.create("GetUIPreferences") != nullptr);
    CHECK(factory.create("GetActiveTasks") != nullptr);
    CHECK(factory.create("GetTaskState") != nullptr);
    CHECK(factory.create("CancelTask") != nullptr);
    CHECK(factory.create("GetAssetAABB") != nullptr);
    CHECK(factory.create("GetProjectAABB") != nullptr);
    CHECK(factory.create("ConvertCadToStl") != nullptr);
    CHECK(factory.create("ConvertCadToSvo") != nullptr);
    CHECK(factory.create("ConvertStlToSvo") != nullptr);
    CHECK(factory.create("ConvertSvoToStl") != nullptr);
    CHECK(factory.create("GenerateLandscape") != nullptr);
    CHECK(factory.create("LoadStlAsset") != nullptr);
    CHECK(factory.create("SaveStlAsset") != nullptr);
    CHECK(factory.create("LoadStepAsset") != nullptr);
    CHECK(factory.create("SaveStepAsset") != nullptr);
    CHECK(factory.create("LoadSvoAsset") != nullptr);
    CHECK(factory.create("SaveVoxelAsset") != nullptr);
    CHECK(factory.create("UpdateCameraPreferences") != nullptr);
    CHECK(factory.create("UpdateUIPreferences") != nullptr);
    CHECK(factory.create("UpdateRenderPreferences") != nullptr);
    CHECK(factory.create("UpdateSVOPreferences") != nullptr);
    CHECK(factory.create("AddNote") != nullptr);
    CHECK(factory.create("GetNotes") != nullptr);
    CHECK(factory.create("UpdateNote") != nullptr);
    CHECK(factory.create("RemoveComponent") != nullptr);
    CHECK(factory.create("Search") != nullptr);
    CHECK(factory.create("GetDocumentation") != nullptr);
    CHECK(factory.getAllCommands().size() == 36);
}
#endif
