#include "UIElement/Menu/MenuEntry.h"

#include <imgui.h>

MenuEntry::MenuEntry(const std::string& label, const std::string& path, const std::string& shortcut, GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows)
  : UIElement(gatedDatabase, undoStack, windows)
  , label(label)
  , path(path)
  , shortcut(shortcut)
  , enabled(true)
  , separatorBefore(false)
{
}

MenuEntry::~MenuEntry() {}

bool MenuEntry::render()
{
    const char* shortcutPtr = shortcut.empty() ? nullptr : shortcut.c_str();
    if (ImGui::MenuItem(label.c_str(), shortcutPtr, false, enabled))
    {
        click();
        return true;
    }
    return false;
}

const std::string& MenuEntry::getLabel() const
{
    return label;
}

void MenuEntry::setLabel(const std::string& newLabel)
{
    label = newLabel;
}

const std::string& MenuEntry::getPath() const
{
    return path;
}

void MenuEntry::setPath(const std::string& newPath)
{
    path = newPath;
}

const std::string& MenuEntry::getShortcut() const
{
    return shortcut;
}

void MenuEntry::setShortcut(const std::string& newShortcut)
{
    shortcut = newShortcut;
}

bool MenuEntry::isEnabled() const
{
    return enabled;
}

void MenuEntry::setEnabled(bool enabledState)
{
    enabled = enabledState;
}

bool MenuEntry::hasSeparatorBefore() const
{
    return separatorBefore;
}

void MenuEntry::setSeparatorBefore(bool separator)
{
    separatorBefore = separator;
}
