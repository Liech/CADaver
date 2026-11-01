#include "UI/Menu/FileMenu.h"

#include "Execution/GatedDatabase.h"
#include "Execution/UndoStack.h"
#include "UIElement/UIElementFactory.h"
#include "Windows/Windows.h"

#include <imgui.h>

FileMenu::FileMenu()
  : initialized(false)
{
}

FileMenu::~FileMenu() {}

void FileMenu::init(GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows, bool& quitRequested)
{
    if (initialized)
    {
        return;
    }

    UIElementFactory factory;
    factory.registerDefaults(gatedDatabase, undoStack, windows, quitRequested);

    auto all = factory.getAllElements();
    for (const auto& elem : all)
    {
        if (elem)
        {
            entries[elem->getType()] = elem;
        }
    }

    initialized = true;
}

UIElement& FileMenu::get(const std::string& type)
{
    auto it = entries.find(type);
    if (it != entries.end() && it->second)
    {
        return *it->second;
    }
    return fallbackElement;
}

void FileMenu::render(GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows, bool& quitRequested)
{
    init(gatedDatabase, undoStack, windows, quitRequested);

    if (ImGui::BeginMenu("File"))
    {
        if (ImGui::BeginMenu("Load"))
        {
            get("STL").render();
            get("STEP").render();
            get("SVO").render();
            ImGui::EndMenu();
        }

        ImGui::Separator();
        get("Preferences").render();

        ImGui::Separator();
        get("Quit").render();

        ImGui::EndMenu();
    }
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("FileMenu/GetAndInvoke")
{
    GatedDatabase db;
    UndoStack     us(db);
    Windows       windows;
    bool          quitFlag = false;
    FileMenu      menu;
    menu.init(db, us, windows, quitFlag);

    REQUIRE(menu.get("SVO").getType() == "SVO");
    REQUIRE(menu.get("STL").getType() == "STL");
    REQUIRE(menu.get("STEP").getType() == "STEP");
    REQUIRE(menu.get("Preferences").getType() == "Preferences");
    REQUIRE(menu.get("Quit").getType() == "Quit");

    menu.get("SVO").click();
    REQUIRE(windows.getOpen().count("LoadSvoAsset") == 1);

    menu.get("Quit").click();
    REQUIRE(quitFlag);

    REQUIRE(menu.get("NonExistent").getType() == "Null");
}
#endif
