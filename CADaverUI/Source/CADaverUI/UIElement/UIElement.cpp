#include "UIElement/UIElement.h"

UIElement::UIElement(GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows)
  : gatedDatabase(gatedDatabase)
  , undoStack(undoStack)
  , windows(windows)
{
}

UIElement::~UIElement() {}

#ifdef ISTESTPROJECT
#include "Execution/GatedDatabase.h"
#include "Execution/UndoStack.h"
#include "Windows/Windows.h"
#include <catch2/catch_test_macros.hpp>

class MockUIElement : public UIElement
{
  public:
    MockUIElement(GatedDatabase& db, UndoStack& us, Windows& w)
      : UIElement(db, us, w)
    {
    }

    virtual ~MockUIElement() {}

    virtual std::string getType() const override
    {
        return "Mock";
    }
    virtual std::string getDescription() const override
    {
        return "Mock Description";
    }
    virtual bool render() override
    {
        return true;
    }
    virtual void click() override {}
};

TEST_CASE("UIElement/ContextReferences")
{
    GatedDatabase db;
    UndoStack     us(db);
    Windows       w;
    MockUIElement elem(db, us, w);

    REQUIRE(elem.getType() == "Mock");
    REQUIRE(elem.getDescription() == "Mock Description");
    REQUIRE(elem.render());
}
#endif
