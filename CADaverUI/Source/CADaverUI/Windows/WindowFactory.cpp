#include "Windows/WindowFactory.h"

#include "Windows/About/AboutWindow.h"
#include "Windows/About/CommandsInfoWindow.h"
#include "Windows/About/WindowsInfoWindow.h"
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
    registerWindow("AboutWindow", []() { return std::make_shared<AboutWindow>(); });
    registerWindow("CommandsInfoWindow", []() { return std::make_shared<CommandsInfoWindow>(); });
    registerWindow("WindowsInfoWindow", []() { return std::make_shared<WindowsInfoWindow>(); });
    registerWindow("CadToStlConversionModal", []() { return std::make_shared<CadToStlConversionModal>(); });
    registerWindow("CadToSvoConversionModal", []() { return std::make_shared<CadToSvoConversionModal>(); });
    registerWindow("StlToSvoConversionModal", []() { return std::make_shared<StlToSvoConversionModal>(); });
    registerWindow("SvoToStlConversionModal", []() { return std::make_shared<SvoToStlConversionModal>(); });
    registerWindow("LandscapeGeneratorWindow", []() { return std::make_shared<LandscapeGeneratorWindow>(); });
    registerWindow("LoadStepAssetWindow", []() { return std::make_shared<LoadStepAssetWindow>(); });
    registerWindow("LoadStlAssetWindow", []() { return std::make_shared<LoadStlAssetWindow>(); });
    registerWindow("LoadSvoAssetWindow", []() { return std::make_shared<LoadSvoAssetWindow>(); });
    registerWindow("SaveStepAssetWindow", []() { return std::make_shared<SaveStepAssetWindow>(); });
    registerWindow("SaveStlAssetWindow", []() { return std::make_shared<SaveStlAssetWindow>(); });
    registerWindow("SaveVoxelAssetWindow", []() { return std::make_shared<SaveVoxelAssetWindow>(); });
    registerWindow("ClearProjectModal", []() { return std::make_shared<ClearProjectModal>(); });
    registerWindow("RemoveAssetModal", []() { return std::make_shared<RemoveAssetModal>(); });
    registerWindow("PreferencesWindow", []() { return std::make_shared<PreferencesWindow>(); });
    registerWindow("TaskNotificationModal", []() { return std::make_shared<TaskNotificationModal>(); });
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("WindowFactory/DefaultRegisteredWindows")
{
    WindowFactory factory;
    CHECK(factory.create("UnknownWindow") == nullptr);
    CHECK(factory.create("AboutWindow") != nullptr);
    CHECK(factory.create("CommandsInfoWindow") != nullptr);
    CHECK(factory.create("WindowsInfoWindow") != nullptr);
    CHECK(factory.create("CadToStlConversionModal") != nullptr);
    CHECK(factory.create("CadToSvoConversionModal") != nullptr);
    CHECK(factory.create("StlToSvoConversionModal") != nullptr);
    CHECK(factory.create("SvoToStlConversionModal") != nullptr);
    CHECK(factory.create("LandscapeGeneratorWindow") != nullptr);
    CHECK(factory.create("LoadStepAssetWindow") != nullptr);
    CHECK(factory.create("LoadStlAssetWindow") != nullptr);
    CHECK(factory.create("LoadSvoAssetWindow") != nullptr);
    CHECK(factory.create("SaveStepAssetWindow") != nullptr);
    CHECK(factory.create("SaveStlAssetWindow") != nullptr);
    CHECK(factory.create("SaveVoxelAssetWindow") != nullptr);
    CHECK(factory.create("ClearProjectModal") != nullptr);
    CHECK(factory.create("PreferencesWindow") != nullptr);
    CHECK(factory.create("RemoveAssetModal") != nullptr);
    CHECK(factory.create("TaskNotificationModal") != nullptr);
    CHECK(factory.getAllWindows().size() == 18);
}
#endif
