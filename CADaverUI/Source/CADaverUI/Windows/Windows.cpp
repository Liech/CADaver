#include "Windows/Windows.h"

#include "Windows/Window.h"
#include "Windows/WindowFactory.h"

#include <imgui.h>
#include <vector>

Windows::Windows() {}

Windows::~Windows() {}

void Windows::setInput(GatedDatabase& gatedDatabaseInput, Defer& deferInput)
{
    gatedDatabase = &gatedDatabaseInput;
    defer         = &deferInput;

    for (auto& pair : windowMap)
    {
        if (pair.second)
        {
            pair.second->setInput(*gatedDatabase, *defer, *this);
        }
    }
}

void Windows::open(const std::string& type)
{
    auto it = windowMap.find(type);
    if (it == windowMap.end() || !it->second)
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
        windowMap[type] = created;
        it              = windowMap.find(type);
    }

    it->second->open();
}

void Windows::remove(const std::string& type)
{
    auto it = windowMap.find(type);
    if (it != windowMap.end())
    {
        if (it->second)
        {
            it->second->close();
        }
        windowMap.erase(it);
    }
}

std::shared_ptr<Window> Windows::get(const std::string& type) const
{
    auto it = windowMap.find(type);
    if (it != windowMap.end())
    {
        return it->second;
    }
    return nullptr;
}

void Windows::render()
{
    std::vector<std::string> keys;
    keys.reserve(windowMap.size());
    for (const auto& pair : windowMap)
    {
        keys.push_back(pair.first);
    }

    std::vector<std::string> closedTypes;

    for (const auto& key : keys)
    {
        auto it = windowMap.find(key);
        if (it == windowMap.end() || !it->second)
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
        windowMap.erase(type);
    }
}

std::set<std::string> Windows::getOpen() const
{
    std::set<std::string> openTypes;
    for (const auto& pair : windowMap)
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

TEST_CASE("Windows/OpenAndRemove")
{
    Windows container;
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
