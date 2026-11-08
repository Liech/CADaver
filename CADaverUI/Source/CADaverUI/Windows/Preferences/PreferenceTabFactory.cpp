#include "Windows/Preferences/PreferenceTabFactory.h"

#include "Windows/Preferences/CameraPreferencesTab.h"
#include "Windows/Preferences/RenderPreferencesTab.h"
#include "Windows/Preferences/SVOPreferencesTab.h"
#include "Windows/Preferences/UIPreferencesTab.h"

#include <algorithm>

PreferenceTabFactory::PreferenceTabFactory()
{
    registerDefaultTabs();
}

PreferenceTabFactory::~PreferenceTabFactory() {}

void PreferenceTabFactory::registerTab(const std::string& name, std::function<std::unique_ptr<PreferenceTab>()> creator)
{
    auto it = std::find(tabNames.begin(), tabNames.end(), name);
    if (it != tabNames.end())
    {
        size_t index       = static_cast<size_t>(std::distance(tabNames.begin(), it));
        tabCreators[index] = std::move(creator);
        return;
    }

    tabNames.push_back(name);
    tabCreators.push_back(std::move(creator));
}

void PreferenceTabFactory::registerDefaultTabs()
{
    clear();
    registerTab("UI", []() { return std::make_unique<UIPreferencesTab>(); });
    registerTab("Camera", []() { return std::make_unique<CameraPreferencesTab>(); });
    registerTab("Render", []() { return std::make_unique<RenderPreferencesTab>(); });
    registerTab("SVO", []() { return std::make_unique<SVOPreferencesTab>(); });
}

void PreferenceTabFactory::clear()
{
    tabNames.clear();
    tabCreators.clear();
}

std::vector<std::string> PreferenceTabFactory::getRegisteredTabs() const
{
    return tabNames;
}

std::vector<std::unique_ptr<PreferenceTab>> PreferenceTabFactory::createTabs() const
{
    std::vector<std::unique_ptr<PreferenceTab>> tabs;
    for (const auto& creator : tabCreators)
    {
        if (creator)
        {
            auto tab = creator();
            if (tab)
            {
                tabs.push_back(std::move(tab));
            }
        }
    }
    return tabs;
}

std::unique_ptr<PreferenceTab> PreferenceTabFactory::createTab(const std::string& name) const
{
    auto it = std::find(tabNames.begin(), tabNames.end(), name);
    if (it != tabNames.end())
    {
        size_t index = static_cast<size_t>(std::distance(tabNames.begin(), it));
        if (index < tabCreators.size() && tabCreators[index])
        {
            return tabCreators[index]();
        }
    }
    return nullptr;
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("PreferenceTabFactory/DefaultsCreation")
{
    PreferenceTabFactory factory;
    auto                 tabs = factory.createTabs();

    CHECK(tabs.size() == 4);
    CHECK(tabs[0]->getTitle() == "UI");
    CHECK(tabs[1]->getTitle() == "Camera");
    CHECK(tabs[2]->getTitle() == "Render");
    CHECK(tabs[3]->getTitle() == "SVO");
}

TEST_CASE("PreferenceTabFactory/SingleTabCreation")
{
    PreferenceTabFactory factory;
    auto                 renderTab = factory.createTab("Render");
    REQUIRE(renderTab != nullptr);
    CHECK(renderTab->getTitle() == "Render");

    auto unknownTab = factory.createTab("Unknown");
    CHECK(unknownTab == nullptr);
}
#endif
