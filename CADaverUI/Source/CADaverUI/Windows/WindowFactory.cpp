#include "Windows/WindowFactory.h"

#include "Windows/About/AboutWindow.h"
#include "Windows/About/CommandsInfoWindow.h"
#include "Windows/About/ConceptsInfoWindow.h"
#include "Windows/About/WindowsInfoWindow.h"
#include "Windows/Asset/TextNoteEditWindow.h"
#include "Windows/Conversion/CadToStlConversionModal.h"
#include "Windows/Conversion/CadToSvoConversionModal.h"
#include "Windows/Conversion/StlToSvoConversionModal.h"
#include "Windows/Conversion/SvoToStlConversionModal.h"
#include "Windows/Generator/LandscapeGeneratorWindow.h"
#include "Windows/IO/LoadStepAssetWindow.h"
#include "Windows/IO/LoadStlAssetWindow.h"
#include "Windows/IO/LoadSvoAssetWindow.h"
#include "Windows/IO/SaveStepAssetWindow.h"
#include "Windows/IO/SaveStlAssetWindow.h"
#include "Windows/IO/SaveVoxelAssetWindow.h"
#include "Windows/Preferences/PreferencesWindow.h"
#include "Windows/Project/ClearProjectModal.h"
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
    registerWindow("StlToSvoConversion", []() { return std::make_shared<StlToSvoConversionModal>(); });
    registerWindow("SvoToStlConversion", []() { return std::make_shared<SvoToStlConversionModal>(); });
    registerWindow("LandscapeGenerator", []() { return std::make_shared<LandscapeGeneratorWindow>(); });
    registerWindow("LoadStepAsset", []() { return std::make_shared<LoadStepAssetWindow>(); });
    registerWindow("LoadStlAsset", []() { return std::make_shared<LoadStlAssetWindow>(); });
    registerWindow("LoadSvoAsset", []() { return std::make_shared<LoadSvoAssetWindow>(); });
    registerWindow("SaveStepAsset", []() { return std::make_shared<SaveStepAssetWindow>(); });
    registerWindow("SaveStlAsset", []() { return std::make_shared<SaveStlAssetWindow>(); });
    registerWindow("SaveVoxelAsset", []() { return std::make_shared<SaveVoxelAssetWindow>(); });
    registerWindow("ClearProject", []() { return std::make_shared<ClearProjectModal>(); });
    registerWindow("RemoveAsset", []() { return std::make_shared<RemoveAssetModal>(); });
    registerWindow("Preferences", []() { return std::make_shared<PreferencesWindow>(); });
    registerWindow("TaskNotification", []() { return std::make_shared<TaskNotificationModal>(); });
    registerWindow("TextNoteEdit", []() { return std::make_shared<TextNoteEditWindow>(); });
    registerWindow("Script", []() { return std::make_shared<ScriptWindow>(); });
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
    CHECK(factory.create("StlToSvoConversion") != nullptr);
    CHECK(factory.create("SvoToStlConversion") != nullptr);
    CHECK(factory.create("LandscapeGenerator") != nullptr);
    CHECK(factory.create("LoadStepAsset") != nullptr);
    CHECK(factory.create("LoadStlAsset") != nullptr);
    CHECK(factory.create("LoadSvoAsset") != nullptr);
    CHECK(factory.create("SaveStepAsset") != nullptr);
    CHECK(factory.create("SaveStlAsset") != nullptr);
    CHECK(factory.create("SaveVoxelAsset") != nullptr);
    CHECK(factory.create("ClearProject") != nullptr);
    CHECK(factory.create("Preferences") != nullptr);
    CHECK(factory.create("RemoveAsset") != nullptr);
    CHECK(factory.create("TaskNotification") != nullptr);
    CHECK(factory.create("TextNoteEdit") != nullptr);
    CHECK(factory.create("Script") != nullptr);
    CHECK(factory.getAllWindows().size() == 21);
}
#endif
