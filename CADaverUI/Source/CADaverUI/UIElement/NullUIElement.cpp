#include "UIElement/NullUIElement.h"

GatedDatabase NullUIElement::dummyDatabase;
UndoStack     NullUIElement::dummyUndoStack(NullUIElement::dummyDatabase);
Windows       NullUIElement::dummyWindows;

NullUIElement::NullUIElement()
  : UIElement(dummyDatabase, dummyUndoStack, dummyWindows)
{
}

NullUIElement::~NullUIElement() {}

std::string NullUIElement::getType() const
{
    return "Null";
}

std::string NullUIElement::getDescription() const
{
    return "";
}

bool NullUIElement::render()
{
    return false;
}

void NullUIElement::click() {}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("NullUIElement/DefaultBehavior")
{
    NullUIElement elem;
    REQUIRE(elem.getType() == "Null");
    REQUIRE(elem.getDescription().empty());
    REQUIRE_FALSE(elem.render());
    elem.click();
}
#endif
