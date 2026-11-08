#include "UI/Menu/TopMenu.h"

#include "Commands/ClickUIElementCommand.h"
#include "Execution/Defer.h"
#include "Execution/GatedDatabase.h"
#include "Execution/UndoStack.h"
#include "Windows/Windows.h"

#include <imgui.h>

TopMenu::TopMenu()
  : factoryInitialized(false)
{
}

TopMenu::~TopMenu() {}

void TopMenu::render(GatedDatabase& gatedDatabase, UndoStack& undoStack, Defer& defer, Windows& windows, bool& quitRequested)
{
    if (!factoryInitialized)
    {
        uiElementFactory.registerDefaults(gatedDatabase, undoStack, windows, quitRequested);
        gatedDatabase.getCommandFactory().registerCommand("ClickUIElement", [this]() { return std::make_unique<ClickUIElementCommand>(uiElementFactory); });
        factoryInitialized = true;
    }

    searchBar.init(gatedDatabase, windows, uiElementFactory);

    if (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_F, false))
    {
        searchBar.focus();
    }

    if (ImGui::BeginMainMenuBar())
    {
        dynamicMenuRenderer.render(uiElementFactory);

        ImGui::SameLine();
        ImGui::Spacing();
        ImGui::SameLine();

        searchBar.render(defer);

        ImGui::EndMainMenuBar();
    }

    handleShortcuts(undoStack);
}

void TopMenu::focusSearchBar()
{
    searchBar.focus();
}

UIElementFactory& TopMenu::getUIElementFactory()
{
    return uiElementFactory;
}

const UIElementFactory& TopMenu::getUIElementFactory() const
{
    return uiElementFactory;
}

void TopMenu::handleShortcuts(UndoStack& undoStack)
{
    if (!ImGui::GetIO().KeyCtrl || ImGui::GetIO().WantTextInput)
    {
        return;
    }

    if (ImGui::IsKeyPressed(ImGuiKey_Z, false))
    {
        if (ImGui::GetIO().KeyShift)
        {
            if (undoStack.canRedo())
            {
                undoStack.redo();
            }
        }
        else
        {
            if (undoStack.canUndo())
            {
                undoStack.undo();
            }
        }
    }
    else if (ImGui::IsKeyPressed(ImGuiKey_Y, false))
    {
        if (undoStack.canRedo())
        {
            undoStack.redo();
        }
    }
    else if (ImGui::IsKeyPressed(ImGuiKey_F, false))
    {
        searchBar.focus();
    }
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("TopMenu/Construction")
{
    TopMenu topMenu;
}
#endif
