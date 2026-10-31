#include "UI/Menu/EditMenu.h"

#include "Execution/Defer.h"
#include "Execution/UndoStack.h"

#include <imgui.h>

EditMenu::EditMenu() {}

EditMenu::~EditMenu() {}

void EditMenu::render(UndoStack& undoStack, Defer& defer)
{
    if (ImGui::BeginMenu("Edit"))
    {
        if (ImGui::MenuItem("Undo", "Ctrl+Z", false, undoStack.canUndo()))
        {
            triggerUndo(undoStack, defer);
        }

        if (ImGui::MenuItem("Redo", "Ctrl+Y", false, undoStack.canRedo()))
        {
            triggerRedo(undoStack, defer);
        }

        ImGui::EndMenu();
    }

    handleShortcuts(undoStack, defer);
}

void EditMenu::handleShortcuts(UndoStack& undoStack, Defer& defer)
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
                triggerRedo(undoStack, defer);
            }
        }
        else
        {
            if (undoStack.canUndo())
            {
                triggerUndo(undoStack, defer);
            }
        }
    }
    else if (ImGui::IsKeyPressed(ImGuiKey_Y, false))
    {
        if (undoStack.canRedo())
        {
            triggerRedo(undoStack, defer);
        }
    }
}

void EditMenu::triggerUndo(UndoStack& undoStack, Defer& defer)
{
    defer.defer([&undoStack]() { undoStack.undo(); });
}

void EditMenu::triggerRedo(UndoStack& undoStack, Defer& defer)
{
    defer.defer([&undoStack]() { undoStack.redo(); });
}
