#include "Windows/Preferences/PreferenceTabFactory.h"

#include "Windows/Preferences/CameraPreferencesTab.h"
#include "Windows/Preferences/RenderPreferencesTab.h"
#include "Windows/Preferences/SVOPreferencesTab.h"
#include "Windows/Preferences/UIPreferencesTab.h"

PreferenceTabFactory& PreferenceTabFactory::getInstance()
{
    static PreferenceTabFactory instance;
    return instance;
}

PreferenceTabFactory::PreferenceTabFactory()
{
    registerDefaultTabs();
}

PreferenceTabFactory::~PreferenceTabFactory() {}

void PreferenceTabFactory::registerTab(std::function<std::unique_ptr<PreferenceTab>()> creator)
{
    tabCreators.push_back(std::move(creator));
}

void PreferenceTabFactory::registerDefaultTabs()
{
    tabCreators.clear();
    registerTab([]() { return std::make_unique<UIPreferencesTab>(); });
    registerTab([]() { return std::make_unique<CameraPreferencesTab>(); });
    registerTab([]() { return std::make_unique<RenderPreferencesTab>(); });
    registerTab([]() { return std::make_unique<SVOPreferencesTab>(); });
}

void PreferenceTabFactory::clear()
{
    tabCreators.clear();
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

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("PreferenceTabFactory/SingletonAndCreation")
{
    auto& factory = PreferenceTabFactory::getInstance();
    auto  tabs    = factory.createTabs();

    CHECK(tabs.size() == 4);
    CHECK(tabs[0]->getTitle() == "UI");
    CHECK(tabs[1]->getTitle() == "Camera");
    CHECK(tabs[2]->getTitle() == "Render");
    CHECK(tabs[3]->getTitle() == "SVO");
}
#endif
