#include "Search/UIElementSearcher.h"

#include "UIElement/Menu/MenuEntry.h"
#include "UIElement/Radio/RadioUIElement.h"

UIElementSearcher::UIElementSearcher(UIElementFactory& factory)
  : factory(factory)
{
}

UIElementSearcher::~UIElementSearcher() {}

std::vector<SearchResult> UIElementSearcher::search(const std::string& query)
{
    std::vector<SearchResult> results;
    if (query.empty())
    {
        return results;
    }

    auto allElements = factory.getAllElements();
    for (const auto& elem : allElements)
    {
        if (!elem)
        {
            continue;
        }

        std::string type        = elem->getType();
        std::string description = elem->getDescription();
        std::string extraText   = "";

        auto menuEntry = std::dynamic_pointer_cast<MenuEntry>(elem);
        if (menuEntry)
        {
            extraText = menuEntry->getPath() + " " + menuEntry->getLabel() + " " + menuEntry->getShortcut();
        }

        auto radioElement = std::dynamic_pointer_cast<RadioUIElement>(elem);
        if (radioElement)
        {
            extraText = radioElement->getGroupName() + " " + radioElement->getLabel();
        }

        if (matcher.matchesAny(query, type + " " + extraText, description))
        {
            std::string title =
              menuEntry ? (menuEntry->getPath().empty() ? menuEntry->getLabel() : menuEntry->getPath()) : (radioElement ? radioElement->getGroupName() + ": " + radioElement->getLabel() : type);
            results.emplace_back(title, "UI", description, [elem]() { elem->click(); });
        }
    }

    return results;
}

#ifdef ISTESTPROJECT
#include "Execution/GatedDatabase.h"
#include "Execution/UndoStack.h"
#include "Windows/Windows.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("UIElementSearcher/SearchAndInvoke")
{
    GatedDatabase    db;
    UndoStack        undoStack(db);
    Windows          windows;
    bool             quitFlag = false;
    UIElementFactory factory;
    factory.registerDefaults(db, undoStack, windows, quitFlag);

    UIElementSearcher searcher(factory);

    auto svoResults = searcher.search("svo");
    REQUIRE(!svoResults.empty());
    REQUIRE(svoResults[0].getCategory() == "UI");

    auto loadStlResults = searcher.search("load stl");
    REQUIRE(!loadStlResults.empty());
    REQUIRE(loadStlResults[0].getCategory() == "UI");

    auto undoResults = searcher.search("undo");
    REQUIRE(!undoResults.empty());
    REQUIRE(undoResults[0].getCategory() == "UI");
    REQUIRE(undoResults[0].getTitle() == "Edit/Undo");

    REQUIRE(svoResults[0].getAction() != nullptr);
    svoResults[0].execute();
    REQUIRE(windows.getOpen().count("LoadSvoAsset") == 1);

    auto wireframeResults = searcher.search("wireframe overlay");
    REQUIRE(!wireframeResults.empty());
    REQUIRE(wireframeResults[0].getTitle() == "Wireframe: Overlay");
    wireframeResults[0].execute();
    REQUIRE(db.get().getPreferences().getRenderPreferences().getWireframeMode() == 1);

    auto cullingResults = searcher.search("cull back");
    REQUIRE(!cullingResults.empty());
    REQUIRE(cullingResults[0].getTitle() == "Culling: Back");
    cullingResults[0].execute();
    REQUIRE(db.get().getPreferences().getRenderPreferences().getCullMode() == 2);

    auto cameraResults = searcher.search("freecam");
    REQUIRE(!cameraResults.empty());
    REQUIRE(cameraResults[0].getTitle() == "Camera Type: Freecam");
    cameraResults[0].execute();
    REQUIRE(db.get().getPreferences().getCameraPreferences().getCameraMode() == 1);

    auto orthoResults = searcher.search("orthogonally");
    REQUIRE(!orthoResults.empty());
    REQUIRE(orthoResults[0].getTitle() == "Projection: Orthogonal");
    orthoResults[0].execute();
    REQUIRE(db.get().getPreferences().getCameraPreferences().isOrthographic());

    auto themeResults = searcher.search("theme light");
    REQUIRE(!themeResults.empty());
    REQUIRE(themeResults[0].getTitle() == "Color Theme: Light");
    themeResults[0].execute();
    REQUIRE(db.get().getPreferences().getUIPreferences().getThemeIndex() == 1);
}
#endif
