#pragma once

#include "UIElement/UIElement.h"

#include <string>

// Base implementation for menu item leaf elements with label, menu path, optional shortcut, and enabled state.
class MenuEntry : public UIElement
{
  public:
    MenuEntry(const std::string& label, const std::string& path, const std::string& shortcut, GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows);
    virtual ~MenuEntry();

    virtual bool render() override;

    const std::string& getLabel() const;
    void               setLabel(const std::string& newLabel);

    const std::string& getPath() const;
    void               setPath(const std::string& newPath);

    const std::string& getShortcut() const;
    void               setShortcut(const std::string& newShortcut);

    bool isEnabled() const;
    void setEnabled(bool enabledState);

    bool hasSeparatorBefore() const;
    void setSeparatorBefore(bool separator);

  private:
    std::string label;
    std::string path;
    std::string shortcut;
    bool        enabled;
    bool        separatorBefore;
};
