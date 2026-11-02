#pragma once

#include "UIElement/UIElement.h"

#include <string>

// Abstract base class for radio button UI elements representing selectable options within a group.
class RadioUIElement : public UIElement
{
  public:
    RadioUIElement(const std::string& groupName, const std::string& label, GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows);
    virtual ~RadioUIElement();

    virtual bool        render() override;
    virtual std::string getType() const override;

    virtual bool isSelected() const = 0;

    const std::string& getGroupName() const;
    void               setGroupName(const std::string& newGroupName);

    const std::string& getLabel() const;
    void               setLabel(const std::string& newLabel);

  private:
    std::string groupName;
    std::string label;
};
