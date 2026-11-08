#include "UIElement/Radio/RadioUIElement.h"

#include <imgui.h>

RadioUIElement::RadioUIElement(const std::string& groupName, const std::string& label, GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows)
  : UIElement(gatedDatabase, undoStack, windows)
  , groupName(groupName)
  , label(label)
{
}

RadioUIElement::~RadioUIElement() {}

bool RadioUIElement::render()
{
    bool selected = isSelected();
    if (ImGui::RadioButton(label.c_str(), selected))
    {
        click();
        return true;
    }
    return false;
}

std::string RadioUIElement::getType() const
{
    return groupName + ": " + label;
}

const std::string& RadioUIElement::getGroupName() const
{
    return groupName;
}

void RadioUIElement::setGroupName(const std::string& newGroupName)
{
    groupName = newGroupName;
}

const std::string& RadioUIElement::getLabel() const
{
    return label;
}

void RadioUIElement::setLabel(const std::string& newLabel)
{
    label = newLabel;
}

#ifdef ISTESTPROJECT
#include "Execution/GatedDatabase.h"
#include "Execution/UndoStack.h"
#include "Windows/Windows.h"
#include <catch2/catch_test_macros.hpp>

class MockRadioUIElement : public RadioUIElement
{
  public:
    MockRadioUIElement(const std::string& group, const std::string& lbl, bool active, GatedDatabase& db, UndoStack& us, Windows& w)
      : RadioUIElement(group, lbl, db, us, w)
      , active(active)
      , clicked(false)
    {
    }

    virtual ~MockRadioUIElement() {}

    std::string getDescription() const override
    {
        return "Mock Radio Element";
    }

    void click() override
    {
        clicked = true;
    }

    bool isSelected() const override
    {
        return active;
    }

    bool wasClicked() const
    {
        return clicked;
    }

  private:
    bool active;
    bool clicked;
};

TEST_CASE("RadioUIElement/PropertiesAndType")
{
    GatedDatabase      db;
    UndoStack          us(db);
    Windows            w;
    MockRadioUIElement elem("Wireframe", "Overlay", true, db, us, w);

    REQUIRE(elem.getGroupName() == "Wireframe");
    REQUIRE(elem.getLabel() == "Overlay");
    REQUIRE(elem.getType() == "Wireframe: Overlay");
    REQUIRE(elem.isSelected());

    elem.setLabel("Only");
    elem.setGroupName("Mesh");
    REQUIRE(elem.getLabel() == "Only");
    REQUIRE(elem.getGroupName() == "Mesh");
    REQUIRE(elem.getType() == "Mesh: Only");

    elem.click();
    REQUIRE(elem.wasClicked());
}
#endif
