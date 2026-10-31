#include "UI/Menu/EditMenu.h"

#include "Commands/Undo/UndoStack.h"
#include "Execution/Defer.h"
#include "Execution/GatedDatabase.h"

#include <imgui.h>

EditMenu::EditMenu() {}

EditMenu::~EditMenu() {}

void EditMenu::render(GatedDatabase& gatedDatabase)
{
    const auto& undoStack = UndoStack::getInstance();

    if (ImGui::BeginMenu("Edit"))
    {
        if (ImGui::MenuItem("Undo", "Ctrl+Z", false, undoStack.canUndo()))
        {
            triggerUndo(gatedDatabase);
        }

        if (ImGui::MenuItem("Redo", "Ctrl+Y", false, undoStack.canRedo()))
        {
            triggerRedo(gatedDatabase);
        }

        ImGui::EndMenu();
    }

    handleShortcuts(gatedDatabase, undoStack);
}

void EditMenu::handleShortcuts(GatedDatabase& gatedDatabase, const UndoStack& undoStack)
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
                triggerRedo(gatedDatabase);
            }
        }
        else
        {
            if (undoStack.canUndo())
            {
                triggerUndo(gatedDatabase);
            }
        }
    }
    else if (ImGui::IsKeyPressed(ImGuiKey_Y, false))
    {
        if (undoStack.canRedo())
        {
            triggerRedo(gatedDatabase);
        }
    }
}

void EditMenu::triggerUndo(GatedDatabase& gatedDatabase)
{
    Defer::getInstance().defer(
      [&gatedDatabase]()
      {
          gatedDatabase.execute({
            {    "type", "Undo" },
            { "version",      1 }
          });
      });
}

void EditMenu::triggerRedo(GatedDatabase& gatedDatabase)
{
    Defer::getInstance().defer(
      [&gatedDatabase]()
      {
          gatedDatabase.execute({
            {    "type", "Redo" },
            { "version",      1 }
          });
      });
}
