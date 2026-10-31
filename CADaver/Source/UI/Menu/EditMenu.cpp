#include "UI/Menu/EditMenu.h"

#include "Execution/Defer.h"
#include "Execution/UndoStack.h"

#include <imgui.h>

EditMenu::EditMenu() {}

EditMenu::~EditMenu() {}

void EditMenu::render(UndoStack& undoStack)
{
    if (ImGui::BeginMenu("Edit"))
    {
        if (ImGui::MenuItem("Undo", "Ctrl+Z", false, undoStack.canUndo()))
        {
            triggerUndo(undoStack);
        }

        if (ImGui::MenuItem("Redo", "Ctrl+Y", false, undoStack.canRedo()))
        {
            triggerRedo(undoStack);
        }

        ImGui::EndMenu();
    }

    handleShortcuts(undoStack);
}

void EditMenu::handleShortcuts(UndoStack& undoStack)
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
                triggerRedo(undoStack);
            }
        }
        else
        {
            if (undoStack.canUndo())
            {
                triggerUndo(undoStack);
            }
        }
    }
    else if (ImGui::IsKeyPressed(ImGuiKey_Y, false))
    {
        if (undoStack.canRedo())
        {
            triggerRedo(undoStack);
        }
    }
}

void EditMenu::triggerUndo(UndoStack& undoStack)
{
    Defer::getInstance().defer([&undoStack]() { undoStack.undo(); });
}

void EditMenu::triggerRedo(UndoStack& undoStack)
{
    Defer::getInstance().defer([&undoStack]() { undoStack.redo(); });
}
