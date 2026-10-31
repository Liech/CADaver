#include "Windows/WindowContainer.h"

#include "Windows/Window.h"
#include "Windows/WindowFactory.h"

#include <imgui.h>
#include <vector>

WindowContainer::WindowContainer() {}

WindowContainer::~WindowContainer() {}

void WindowContainer::setInput(GatedDatabase& gatedDatabaseInput, Defer& deferInput)
{
    gatedDatabase = &gatedDatabaseInput;
    defer         = &deferInput;

    for (auto& pair : windows)
    {
        if (pair.second)
        {
            pair.second->setInput(*gatedDatabase, *defer, *this);
        }
    }
}

void WindowContainer::open(const std::string& type)
{
    auto it = windows.find(type);
    if (it == windows.end() || !it->second)
    {
        WindowFactory factory;
        auto          created = factory.create(type);
        if (!created)
        {
            return;
        }
        if (gatedDatabase && defer)
        {
            created->setInput(*gatedDatabase, *defer, *this);
        }
        windows[type] = created;
        it            = windows.find(type);
    }

    it->second->open();
}

void WindowContainer::remove(const std::string& type)
{
    auto it = windows.find(type);
    if (it != windows.end())
    {
        if (it->second)
        {
            it->second->close();
        }
        windows.erase(it);
    }
}

std::shared_ptr<Window> WindowContainer::get(const std::string& type) const
{
    auto it = windows.find(type);
    if (it != windows.end())
    {
        return it->second;
    }
    return nullptr;
}

void WindowContainer::render()
{
    std::vector<std::string> keys;
    keys.reserve(windows.size());
    for (const auto& pair : windows)
    {
        keys.push_back(pair.first);
    }

    std::vector<std::string> closedTypes;

    for (const auto& key : keys)
    {
        auto it = windows.find(key);
        if (it == windows.end() || !it->second)
        {
            closedTypes.push_back(key);
            continue;
        }

        if (gatedDatabase && defer)
        {
            it->second->setInput(*gatedDatabase, *defer, *this);
        }

        if (it->second->consumeFocus())
        {
            ImGui::SetNextWindowFocus();
        }

        it->second->render();

        if (!it->second->isOpen())
        {
            closedTypes.push_back(key);
        }
    }

    for (const auto& type : closedTypes)
    {
        windows.erase(type);
    }
}

std::set<std::string> WindowContainer::getOpen() const
{
    std::set<std::string> openTypes;
    for (const auto& pair : windows)
    {
        if (pair.second && pair.second->isOpen())
        {
            openTypes.insert(pair.first);
        }
    }
    return openTypes;
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("WindowContainer/OpenAndRemove")
{
    WindowContainer container;
    CHECK(container.getOpen().empty());
    CHECK(container.get("LandscapeGeneratorWindow") == nullptr);

    container.open("LandscapeGeneratorWindow");
    CHECK(container.getOpen().count("LandscapeGeneratorWindow") == 1);
    REQUIRE(container.get("LandscapeGeneratorWindow") != nullptr);
    CHECK(container.get("LandscapeGeneratorWindow")->isOpen());

    container.remove("LandscapeGeneratorWindow");
    CHECK(container.getOpen().empty());
    CHECK(container.get("LandscapeGeneratorWindow") == nullptr);
}
#endif
