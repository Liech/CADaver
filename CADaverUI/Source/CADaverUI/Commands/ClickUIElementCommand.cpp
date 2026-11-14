#include "Commands/ClickUIElementCommand.h"

#include "UIElement/Menu/MenuEntry.h"
#include "UIElement/UIElementFactory.h"

ClickUIElementCommand::ClickUIElementCommand(UIElementFactory& factory)
  : factory(factory)
{
}

ClickUIElementCommand::~ClickUIElementCommand() {}

std::string ClickUIElementCommand::getType() const
{
    return "ClickUIElement";
}

int ClickUIElementCommand::getVersion() const
{
    return 1;
}

std::string ClickUIElementCommand::getDescription() const
{
    return "Command: ClickUIElement\n"
           "Version: 1\n"
           "Purpose: Programmatically clicks a registered UI element or menu entry by type, path, or label.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"ClickUIElement\",\n"
           "  \"version\": 1,\n"
           "  \"target\": \"<string>\" // matches type (e.g. \"LoadAsset\", \"Undo\", \"ViewHome\"), path (\"File/Load\"), or label (\"Load...\")\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: None\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"ClickUIElement\",\n"
           "  \"version\": 1,\n"
           "  \"target\": \"<string>\",\n"
           "  \"matchedType\": \"<string>\"\n"
           "}";
}

void ClickUIElementCommand::setInput(const Polyglot::Node& node)
{
    if (node.isMap())
    {
        const auto& map = node.asMap();
        auto        it  = map.find("target");
        if (it != map.end() && it->second.isString())
        {
            target = it->second.asString();
            return;
        }
    }
    target.clear();
}

const std::string& ClickUIElementCommand::getTarget() const
{
    return target;
}

nlohmann::json ClickUIElementCommand::execute(Database& database)
{
    if (target.empty())
    {
        return createErrorResponse("MissingTarget", "Missing 'target' parameter");
    }

    auto allElements = factory.getAllElements();
    for (const auto& elem : allElements)
    {
        if (!elem)
        {
            continue;
        }

        bool matches = false;
        if (elem->getType() == target)
        {
            matches = true;
        }

        auto menuEntry = std::dynamic_pointer_cast<MenuEntry>(elem);
        if (menuEntry)
        {
            if (menuEntry->getPath() == target || menuEntry->getLabel() == target)
            {
                matches = true;
            }
        }

        if (matches)
        {
            elem->click();
            return createResponse({
              {      "target",          target },
              { "matchedType", elem->getType() }
            });
        }
    }

    return createErrorResponse("NotFound", "UIElement '" + target + "' not found");
}

#ifdef ISTESTPROJECT
#include "Database/Database.h"
#include "Execution/GatedDatabase.h"
#include "Execution/UndoStack.h"
#include "Windows/Windows.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("ClickUIElementCommand/Execute")
{
    GatedDatabase    gatedDb;
    UndoStack        undoStack(gatedDb);
    Windows          windows;
    bool             quitFlag = false;
    UIElementFactory factory;
    factory.registerDefaults(gatedDb, undoStack, windows, quitFlag);

    ClickUIElementCommand cmd(factory);

    // Test missing target
    cmd.setInput(Polyglot::Node(std::map<std::string, Polyglot::Node>{}));
    Database db;
    auto     resMissing = cmd.execute(db);
    REQUIRE(resMissing["type"] == "Error");
    REQUIRE(resMissing["command"] == "ClickUIElement");
    REQUIRE(resMissing["errorType"] == "MissingTarget");

    // Test click by type
    cmd.setInput({
      { "target", "Preferences" }
    });
    auto resType = cmd.execute(db);
    REQUIRE(resType["type"] == "ClickUIElement");
    REQUIRE(windows.getOpen().count("Preferences") == 1);

    // Test click by path
    cmd.setInput({
      { "target", "File/Load" }
    });
    auto resPath = cmd.execute(db);
    REQUIRE(resPath["type"] == "ClickUIElement");
    REQUIRE(windows.getOpen().count("LoadAsset") == 1);

    // Test click non-existent
    cmd.setInput({
      { "target", "NonExistent" }
    });
    auto resNonExistent = cmd.execute(db);
    REQUIRE(resNonExistent["type"] == "Error");
    REQUIRE(resNonExistent["command"] == "ClickUIElement");
    REQUIRE(resNonExistent["errorType"] == "NotFound");
}
#endif
