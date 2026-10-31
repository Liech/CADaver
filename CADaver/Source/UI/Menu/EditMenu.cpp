#include "UI/Menu/EditMenu.h"

#include "Commands/Undo/UndoStack.h"
#include "Execution/Defer.h"
#include "Execution/GatedDatabase.h"

#include <imgui.h>

EditMenu::EditMenu() {}

EditMenu::~EditMenu() {}

void EditMenu::render()
{
    const auto& undoStack = UndoStack::getInstance();

    if (ImGui::BeginMenu("Edit"))
    {
        if (ImGui::MenuItem("Undo", "Ctrl+Z", false, undoStack.canUndo()))
        {
            triggerUndo();
        }

        if (ImGui::MenuItem("Redo", "Ctrl+Y", false, undoStack.canRedo()))
        {
            triggerRedo();
        }

        ImGui::EndMenu();
    }

    handleShortcuts(undoStack);
}

void EditMenu::handleShortcuts(const UndoStack& undoStack)
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
                triggerRedo();
            }
        }
        else
        {
            if (undoStack.canUndo())
            {
                triggerUndo();
            }
        }
    }
    else if (ImGui::IsKeyPressed(ImGuiKey_Y, false))
    {
        if (undoStack.canRedo())
        {
            triggerRedo();
        }
    }
}

void EditMenu::triggerUndo()
{
    Defer::getInstance().defer(
      []()
      {
          GatedDatabase::getInstance().execute({
            {    "type", "Undo" },
            { "version",      1 }
          });
      });
}

void EditMenu::triggerRedo()
{
    Defer::getInstance().defer(
      []()
      {
          GatedDatabase::getInstance().execute({
            {    "type", "Redo" },
            { "version",      1 }
          });
      });
}
