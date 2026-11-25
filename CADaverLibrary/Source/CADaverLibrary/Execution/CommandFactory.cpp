#include "Execution/CommandFactory.h"

#include "Commands/Component/AddComponentCommand.h"
#include "Commands/Component/AddTextureComponentCommand.h"
#include "Commands/Component/GetComponentCommand.h"
#include "Commands/Component/RemoveComponentCommand.h"
#include "Commands/Component/UpdateComponentCommand.h"
#include "Commands/Conversion/ConvertCadToStlCommand.h"
#include "Commands/Conversion/ConvertCadToSvoCommand.h"
#include "Commands/Conversion/ConvertClusteringToCadCommand.h"
#include "Commands/Conversion/ConvertStlToCadCommand.h"
#include "Commands/Conversion/ConvertStlToSvoCommand.h"
#include "Commands/Conversion/ConvertSvoToStlCommand.h"
#include "Commands/Conversion/ConvertSvoToStlMarchingCubesCommand.h"
#include "Commands/Conversion/ConvertSvoToStlSurfaceNetsCommand.h"
#include "Commands/Generator/GenerateBaseShapeCommand.h"
#include "Commands/Generator/GenerateFunctionSvoCommand.h"
#include "Commands/Generator/GenerateLandscapeCommand.h"
#include "Commands/Geometry/CalculateConnectedComponentsCommand.h"
#include "Commands/Geometry/ClusterByNormalCommand.h"
#include "Commands/Geometry/GetAssetAABBCommand.h"
#include "Commands/Geometry/GetProjectAABBCommand.h"
#include "Commands/Geometry/SimplifyMeshCommand.h"
#include "Commands/Geometry/SplitClustersToAssetsCommand.h"
#include "Commands/IO/LoadAssetCommand.h"
#include "Commands/IO/SaveAssetCommand.h"
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
#include "Commands/Project/GetAssetVisibilityCommand.h"
#include "Commands/Project/GetAssetsCommand.h"
#include "Commands/Project/RemoveAssetCommand.h"
#include "Commands/Project/SetAssetVisibilityCommand.h"
#include "Commands/Search/GetDocumentationCommand.h"
#include "Commands/Search/SearchCommand.h"
#include "Commands/Selection/ClearSelectionCommand.h"
#include "Commands/Selection/GetAssetSelectionCommand.h"
#include "Commands/Selection/GetSelectedAssetsCommand.h"
#include "Commands/Selection/PickAssetCommand.h"
#include "Commands/Selection/SelectAssetCommand.h"
#include "Commands/Task/CancelTaskCommand.h"
#include "Commands/Task/GetActiveTasksCommand.h"
#include "Commands/Task/GetTaskStateCommand.h"
#include "Commands/Time/GetCurrentTimeCommand.h"
#include "Commands/Time/StepTimeCommand.h"
#include "Commands/Transform/GetTransformationAssetCommand.h"
#include "Commands/Transform/TransformAssetCommand.h"

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
    registerCommand("AddComponent", []() { return std::make_unique<AddComponentCommand>(); });
    registerCommand("AddTextureComponent", []() { return std::make_unique<AddTextureComponentCommand>(); });
    registerCommand("GetComponent", []() { return std::make_unique<GetComponentCommand>(); });
    registerCommand("UpdateComponent", []() { return std::make_unique<UpdateComponentCommand>(); });
    registerCommand("RemoveComponent", []() { return std::make_unique<RemoveComponentCommand>(); });
    registerCommand("RemoveAsset", []() { return std::make_unique<RemoveAssetCommand>(); });
    registerCommand("ClearProject", []() { return std::make_unique<ClearProjectCommand>(); });
    registerCommand("ConvertCadToStl", []() { return std::make_unique<ConvertCadToStlCommand>(); });
    registerCommand("ConvertCadToSvo", []() { return std::make_unique<ConvertCadToSvoCommand>(); });
    registerCommand("ConvertClusteringToCad", []() { return std::make_unique<ConvertClusteringToCadCommand>(); });
    registerCommand("ConvertStlToCad", []() { return std::make_unique<ConvertStlToCadCommand>(); });
    registerCommand("ConvertStlToSvo", []() { return std::make_unique<ConvertStlToSvoCommand>(); });
    registerCommand("ConvertSvoToStl", []() { return std::make_unique<ConvertSvoToStlCommand>(); });
    registerCommand("ConvertSvoToStlMarchingCubes", []() { return std::make_unique<ConvertSvoToStlMarchingCubesCommand>(); });
    registerCommand("ConvertSvoToStlSurfaceNets", []() { return std::make_unique<ConvertSvoToStlSurfaceNetsCommand>(); });
    registerCommand("GenerateBaseShape", []() { return std::make_unique<GenerateBaseShapeCommand>(); });
    registerCommand("GenerateFunctionSvo", []() { return std::make_unique<GenerateFunctionSvoCommand>(); });
    registerCommand("GenerateLandscape", []() { return std::make_unique<GenerateLandscapeCommand>(); });
    registerCommand("LoadAsset", []() { return std::make_unique<LoadAssetCommand>(); });
    registerCommand("SaveAsset", []() { return std::make_unique<SaveAssetCommand>(); });
    registerCommand("UpdateCameraPreferences", []() { return std::make_unique<UpdateCameraPreferencesCommand>(); });
    registerCommand("UpdateUIPreferences", []() { return std::make_unique<UpdateUIPreferencesCommand>(); });
    registerCommand("UpdateRenderPreferences", []() { return std::make_unique<UpdateRenderPreferencesCommand>(); });
    registerCommand("UpdateSVOPreferences", []() { return std::make_unique<UpdateSVOPreferencesCommand>(); });
    registerCommand("GetAssetCount", []() { return std::make_unique<GetAssetCountCommand>(); });
    registerCommand("GetAssetInfo", []() { return std::make_unique<GetAssetInfoCommand>(); });
    registerCommand("GetAssetNames", []() { return std::make_unique<GetAssetNamesCommand>(); });
    registerCommand("GetAssetVisibility", []() { return std::make_unique<GetAssetVisibilityCommand>(); });
    registerCommand("GetAssets", []() { return std::make_unique<GetAssetsCommand>(); });
    registerCommand("SetAssetVisibility", []() { return std::make_unique<SetAssetVisibilityCommand>(); });
    registerCommand("GetCameraPreferences", []() { return std::make_unique<GetCameraPreferencesCommand>(); });
    registerCommand("GetRenderPreferences", []() { return std::make_unique<GetRenderPreferencesCommand>(); });
    registerCommand("GetSVOPreferences", []() { return std::make_unique<GetSVOPreferencesCommand>(); });
    registerCommand("GetUIPreferences", []() { return std::make_unique<GetUIPreferencesCommand>(); });
    registerCommand("GetActiveTasks", []() { return std::make_unique<GetActiveTasksCommand>(); });
    registerCommand("GetTaskState", []() { return std::make_unique<GetTaskStateCommand>(); });
    registerCommand("CancelTask", []() { return std::make_unique<CancelTaskCommand>(); });
    registerCommand("CalculateConnectedComponents", []() { return std::make_unique<CalculateConnectedComponentsCommand>(); });
    registerCommand("ClusterByNormal", []() { return std::make_unique<ClusterByNormalCommand>(); });
    registerCommand("SimplifyMesh", []() { return std::make_unique<SimplifyMeshCommand>(); });
    registerCommand("SplitClustersToAssets", []() { return std::make_unique<SplitClustersToAssetsCommand>(); });
    registerCommand("GetAssetAABB", []() { return std::make_unique<GetAssetAABBCommand>(); });
    registerCommand("GetProjectAABB", []() { return std::make_unique<GetProjectAABBCommand>(); });
    registerCommand("Search", []() { return std::make_unique<SearchCommand>(); });
    registerCommand("GetDocumentation", []() { return std::make_unique<GetDocumentationCommand>(); });
    registerCommand("SelectAsset", []() { return std::make_unique<SelectAssetCommand>(); });
    registerCommand("ClearSelection", []() { return std::make_unique<ClearSelectionCommand>(); });
    registerCommand("GetAssetSelection", []() { return std::make_unique<GetAssetSelectionCommand>(); });
    registerCommand("TransformAsset", []() { return std::make_unique<TransformAssetCommand>(); });
    registerCommand("GetTransformationAsset", []() { return std::make_unique<GetTransformationAssetCommand>(); });
    registerCommand("GetSelectedAssets", []() { return std::make_unique<GetSelectedAssetsCommand>(); });
    registerCommand("PickAsset", []() { return std::make_unique<PickAssetCommand>(); });
    registerCommand("StepTime", []() { return std::make_unique<StepTimeCommand>(); });
    registerCommand("GetCurrentTime", []() { return std::make_unique<GetCurrentTimeCommand>(); });
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
    CHECK(factory.create("GetAssetVisibility") != nullptr);
    CHECK(factory.create("GetAssets") != nullptr);
    CHECK(factory.create("SetAssetVisibility") != nullptr);
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
    CHECK(factory.create("ConvertSvoToStlMarchingCubes") != nullptr);
    CHECK(factory.create("ConvertSvoToStlSurfaceNets") != nullptr);
    CHECK(factory.create("GenerateLandscape") != nullptr);
    CHECK(factory.create("LoadAsset") != nullptr);
    CHECK(factory.create("SaveAsset") != nullptr);
    CHECK(factory.create("UpdateCameraPreferences") != nullptr);
    CHECK(factory.create("UpdateUIPreferences") != nullptr);
    CHECK(factory.create("UpdateRenderPreferences") != nullptr);
    CHECK(factory.create("UpdateSVOPreferences") != nullptr);
    CHECK(factory.create("AddComponent") != nullptr);
    CHECK(factory.create("AddTextureComponent") != nullptr);
    CHECK(factory.create("GetComponent") != nullptr);
    CHECK(factory.create("UpdateComponent") != nullptr);
    CHECK(factory.create("RemoveComponent") != nullptr);
    CHECK(factory.create("Search") != nullptr);
    CHECK(factory.create("GetDocumentation") != nullptr);
    CHECK(factory.create("SelectAsset") != nullptr);
    CHECK(factory.create("ClearSelection") != nullptr);
    CHECK(factory.create("GetAssetSelection") != nullptr);
    CHECK(factory.create("TransformAsset") != nullptr);
    CHECK(factory.create("GetTransformationAsset") != nullptr);
    CHECK(factory.create("GetSelectedAssets") != nullptr);
    CHECK(factory.create("PickAsset") != nullptr);
    CHECK(factory.create("CalculateConnectedComponents") != nullptr);
    CHECK(factory.create("ClusterByNormal") != nullptr);
    CHECK(factory.create("SimplifyMesh") != nullptr);
    CHECK(factory.create("SplitClustersToAssets") != nullptr);
    CHECK(factory.create("ConvertClusteringToCad") != nullptr);
    CHECK(factory.create("GenerateBaseShape") != nullptr);
    CHECK(factory.create("GenerateFunctionSvo") != nullptr);
    CHECK(factory.create("StepTime") != nullptr);
    CHECK(factory.create("GetCurrentTime") != nullptr);
    CHECK(factory.getAllCommands().size() == 54);
}
#endif
