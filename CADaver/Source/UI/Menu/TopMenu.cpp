#include "UI/Menu/TopMenu.h"

#include "Commands/ClickUIElementCommand.h"
#include "Execution/Defer.h"
#include "Execution/GatedDatabase.h"
#include "Execution/UndoStack.h"
#include "Windows/Windows.h"

#include <format>
#include <imgui.h>
#include <string>

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

        float       fps      = ImGui::GetIO().Framerate;
        float       frameMs  = (fps > 0.0f) ? (1000.0f / fps) : 0.0f;
        std::string fpsLabel = std::format("{:.1f} FPS ({:.2f} ms)", fps, frameMs);
        float       textW    = ImGui::CalcTextSize(fpsLabel.c_str()).x;
        float       rightX   = ImGui::GetWindowWidth() - textW - ImGui::GetStyle().ItemSpacing.x * 2.0f;
        if (rightX > ImGui::GetCursorPosX())
        {
            ImGui::SetCursorPosX(rightX);
        }
        ImGui::TextUnformatted(fpsLabel.c_str());

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
