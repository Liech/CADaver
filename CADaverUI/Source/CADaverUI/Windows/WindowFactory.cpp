#include "Windows/WindowFactory.h"

#include "Windows/About/AboutWindow.h"
#include "Windows/About/CommandsInfoWindow.h"
#include "Windows/About/ConceptsInfoWindow.h"
#include "Windows/About/WindowsInfoWindow.h"
#include "Windows/Asset/TextNoteEditWindow.h"
#include "Windows/Console/ConsoleWindow.h"
#include "Windows/Conversion/CadToStlConversionModal.h"
#include "Windows/Conversion/CadToSvoConversionModal.h"
#include "Windows/Conversion/ConvertClusteringToCadWindow.h"
#include "Windows/Conversion/ConvertStlToCadWindow.h"
#include "Windows/Conversion/StlToSvoConversionModal.h"
#include "Windows/Conversion/SvoToStlConversionModal.h"
#include "Windows/Conversion/SvoToStlMarchingCubesConversionModal.h"
#include "Windows/Conversion/SvoToStlSurfaceNetsConversionModal.h"
#include "Windows/Generator/BaseShapeGeneratorWindow.h"
#include "Windows/Generator/LandscapeGeneratorWindow.h"
#include "Windows/Geometry/CalculateConnectedComponentsWindow.h"
#include "Windows/Geometry/ClusterByNormalWindow.h"
#include "Windows/Geometry/SimplifyMeshWindow.h"
#include "Windows/IO/LoadAssetWindow.h"
#include "Windows/IO/SaveAssetWindow.h"
#include "Windows/Preferences/PreferencesWindow.h"
#include "Windows/Project/ClearProjectModal.h"
#include "Windows/Project/ProgressWindow.h"
#include "Windows/Project/RemoveAssetModal.h"
#include "Windows/Project/TaskNotificationModal.h"
#include "Windows/Script/ScriptWindow.h"
#include "Windows/Window.h"

#include <algorithm>

WindowFactory::WindowFactory()
{
    registerWindows();
}

WindowFactory::~WindowFactory() {}

std::shared_ptr<Window> WindowFactory::create(const std::string& type) const
{
    auto it = creators.find(type);
    if (it != creators.end() && it->second)
    {
        return it->second();
    }

    return nullptr;
}

std::vector<std::shared_ptr<Window>> WindowFactory::getAllWindows() const
{
    std::vector<std::shared_ptr<Window>> windows;
    for (const auto& name : registeredOrder)
    {
        auto it = creators.find(name);
        if (it != creators.end() && it->second)
        {
            auto win = it->second();
            if (win)
            {
                windows.push_back(std::move(win));
            }
        }
    }
    return windows;
}

void WindowFactory::registerWindow(const std::string& name, std::function<std::shared_ptr<Window>()> creator)
{
    if (std::find(registeredOrder.begin(), registeredOrder.end(), name) == registeredOrder.end())
    {
        registeredOrder.push_back(name);
    }
    creators[name] = std::move(creator);
}

void WindowFactory::registerWindows()
{
    registerWindow("About", []() { return std::make_shared<AboutWindow>(); });
    registerWindow("CommandsInfo", []() { return std::make_shared<CommandsInfoWindow>(); });
    registerWindow("ConceptsInfo", []() { return std::make_shared<ConceptsInfoWindow>(); });
    registerWindow("WindowsInfo", []() { return std::make_shared<WindowsInfoWindow>(); });
    registerWindow("CadToStlConversion", []() { return std::make_shared<CadToStlConversionModal>(); });
    registerWindow("CadToSvoConversion", []() { return std::make_shared<CadToSvoConversionModal>(); });
    registerWindow("ConvertClusteringToCad", []() { return std::make_shared<ConvertClusteringToCadWindow>(); });
    registerWindow("ConvertStlToCad", []() { return std::make_shared<ConvertStlToCadWindow>(); });
    registerWindow("StlToSvoConversion", []() { return std::make_shared<StlToSvoConversionModal>(); });
    registerWindow("SvoToStlConversion", []() { return std::make_shared<SvoToStlConversionModal>(); });
    registerWindow("SvoToStlMarchingCubesConversion", []() { return std::make_shared<SvoToStlMarchingCubesConversionModal>(); });
    registerWindow("SvoToStlSurfaceNetsConversion", []() { return std::make_shared<SvoToStlSurfaceNetsConversionModal>(); });
    registerWindow("BaseShapeGenerator", []() { return std::make_shared<BaseShapeGeneratorWindow>(); });
    registerWindow("LandscapeGenerator", []() { return std::make_shared<LandscapeGeneratorWindow>(); });
    registerWindow("CalculateConnectedComponents", []() { return std::make_shared<CalculateConnectedComponentsWindow>(); });
    registerWindow("ClusterByNormal", []() { return std::make_shared<ClusterByNormalWindow>(); });
    registerWindow("SimplifyMesh", []() { return std::make_shared<SimplifyMeshWindow>(); });
    registerWindow("LoadAsset", []() { return std::make_shared<LoadAssetWindow>(); });
    registerWindow("SaveAsset", []() { return std::make_shared<SaveAssetWindow>(); });
    registerWindow("ClearProject", []() { return std::make_shared<ClearProjectModal>(); });
    registerWindow("Progress", []() { return std::make_shared<ProgressWindow>(); });
    registerWindow("RemoveAsset", []() { return std::make_shared<RemoveAssetModal>(); });
    registerWindow("Preferences", []() { return std::make_shared<PreferencesWindow>(); });
    registerWindow("TaskNotification", []() { return std::make_shared<TaskNotificationModal>(); });
    registerWindow("TextNoteEdit", []() { return std::make_shared<TextNoteEditWindow>(); });
    registerWindow("Script", []() { return std::make_shared<ScriptWindow>(); });
    registerWindow("Console", []() { return std::make_shared<ConsoleWindow>(); });
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("WindowFactory/DefaultRegisteredWindows")
{
    WindowFactory factory;
    CHECK(factory.create("UnknownWindow") == nullptr);
    CHECK(factory.create("About") != nullptr);
    CHECK(factory.create("CommandsInfo") != nullptr);
    CHECK(factory.create("ConceptsInfo") != nullptr);
    CHECK(factory.create("WindowsInfo") != nullptr);
    CHECK(factory.create("CadToStlConversion") != nullptr);
    CHECK(factory.create("CadToSvoConversion") != nullptr);
    CHECK(factory.create("ConvertClusteringToCad") != nullptr);
    CHECK(factory.create("StlToSvoConversion") != nullptr);
    CHECK(factory.create("SvoToStlConversion") != nullptr);
    CHECK(factory.create("SvoToStlMarchingCubesConversion") != nullptr);
    CHECK(factory.create("SvoToStlSurfaceNetsConversion") != nullptr);
    CHECK(factory.create("LandscapeGenerator") != nullptr);
    CHECK(factory.create("CalculateConnectedComponents") != nullptr);
    CHECK(factory.create("ClusterByNormal") != nullptr);
    CHECK(factory.create("SimplifyMesh") != nullptr);
    CHECK(factory.create("LoadAsset") != nullptr);
    CHECK(factory.create("SaveAsset") != nullptr);
    CHECK(factory.create("ClearProject") != nullptr);
    CHECK(factory.create("Progress") != nullptr);
    CHECK(factory.create("Preferences") != nullptr);
    CHECK(factory.create("RemoveAsset") != nullptr);
    CHECK(factory.create("TaskNotification") != nullptr);
    CHECK(factory.create("TextNoteEdit") != nullptr);
    CHECK(factory.create("Script") != nullptr);
    CHECK(factory.create("BaseShapeGenerator") != nullptr);
    CHECK(factory.create("Console") != nullptr);
    CHECK(factory.getAllWindows().size() == 27);
}
#endif
