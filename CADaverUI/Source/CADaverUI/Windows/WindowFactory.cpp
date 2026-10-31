#include "Windows/WindowFactory.h"

#include "Windows/Conversion/CadToStlConversionModal.h"
#include "Windows/Conversion/CadToSvoConversionModal.h"
#include "Windows/Conversion/StlToSvoConversionModal.h"
#include "Windows/Conversion/SvoToStlConversionModal.h"
#include "Windows/Generator/LandscapeGeneratorWindow.h"
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
    registerWindow("CadToStlConversionModal", []() { return std::make_shared<CadToStlConversionModal>(); });
    registerWindow("CadToSvoConversionModal", []() { return std::make_shared<CadToSvoConversionModal>(); });
    registerWindow("StlToSvoConversionModal", []() { return std::make_shared<StlToSvoConversionModal>(); });
    registerWindow("SvoToStlConversionModal", []() { return std::make_shared<SvoToStlConversionModal>(); });
    registerWindow("LandscapeGeneratorWindow", []() { return std::make_shared<LandscapeGeneratorWindow>(); });
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("WindowFactory/DefaultRegisteredWindows")
{
    WindowFactory factory;
    CHECK(factory.create("UnknownWindow") == nullptr);
    CHECK(factory.create("CadToStlConversionModal") != nullptr);
    CHECK(factory.create("CadToSvoConversionModal") != nullptr);
    CHECK(factory.create("StlToSvoConversionModal") != nullptr);
    CHECK(factory.create("SvoToStlConversionModal") != nullptr);
    CHECK(factory.create("LandscapeGeneratorWindow") != nullptr);
    CHECK(factory.getAllWindows().size() == 5);
}
#endif
