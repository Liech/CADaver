#include "Search/PreferenceTabSearcher.h"

#include "Windows/Preferences/PreferenceTab.h"
#include "Windows/Preferences/PreferenceTabFactory.h"
#include "Windows/Preferences/PreferencesWindow.h"
#include "Windows/Windows.h"

PreferenceTabSearcher::PreferenceTabSearcher(Windows& windows)
  : windows(windows)
{
}

PreferenceTabSearcher::~PreferenceTabSearcher() {}

std::vector<SearchResult> PreferenceTabSearcher::search(const std::string& query)
{
    std::vector<SearchResult> results;
    if (query.empty())
    {
        return results;
    }

    PreferenceTabFactory factory;
    auto                 tabs = factory.createTabs();

    for (const auto& tab : tabs)
    {
        if (!tab)
        {
            continue;
        }

        std::string rawTitle    = tab->getTitle();
        std::string fullTitle   = rawTitle + " Preferences";
        std::string description = tab->getDescription();

        if (matcher.matchesAny(query, fullTitle, description) || matcher.matchesAny(query, rawTitle, ""))
        {
            results.emplace_back(fullTitle,
                                 "Preferences",
                                 description,
                                 [this, rawTitle]()
                                 {
                                     windows.open("Preferences");
                                     auto window = windows.get("Preferences");
                                     if (auto prefs = std::dynamic_pointer_cast<PreferencesWindow>(window))
                                     {
                                         prefs->selectTab(rawTitle);
                                     }
                                 });
        }
    }

    return results;
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("PreferenceTabSearcher/Matching")
{
    Windows               windows;
    PreferenceTabSearcher searcher(windows);

    auto resultsRender = searcher.search("wireframe");
    REQUIRE(!resultsRender.empty());
    CHECK(resultsRender[0].getTitle() == "Render Preferences");
    CHECK(resultsRender[0].getCategory() == "Preferences");

    auto resultsCamera = searcher.search("camera");
    REQUIRE(!resultsCamera.empty());
    CHECK(resultsCamera[0].getTitle() == "Camera Preferences");

    auto resultsSVO = searcher.search("lod");
    REQUIRE(!resultsSVO.empty());
    CHECK(resultsSVO[0].getTitle() == "SVO Preferences");
}

TEST_CASE("PreferenceTabSearcher/ActionExecution")
{
    Windows               windows;
    PreferenceTabSearcher searcher(windows);

    auto results = searcher.search("Render");
    REQUIRE(!results.empty());

    auto action = results[0].getAction();
    REQUIRE(action != nullptr);
    action();

    CHECK(windows.getOpen().count("Preferences") == 1);
    auto window = windows.get("Preferences");
    auto prefs  = std::dynamic_pointer_cast<PreferencesWindow>(window);
    REQUIRE(prefs != nullptr);
    CHECK(prefs->getSelectedTab() == "Render");
}
#endif
