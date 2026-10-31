#include "Windows/WindowFactory.h"

#include "Windows/Window.h"

#include <algorithm>

WindowFactory::WindowFactory() {}

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

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("WindowFactory/CreateUnknownType")
{
    WindowFactory factory;
    CHECK(factory.create("UnknownWindow") == nullptr);
    CHECK(factory.getAllWindows().empty());
}
#endif
