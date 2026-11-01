#include "UIElement/Menu/DynamicMenuRenderer.h"

#include "UIElement/Menu/DynamicMenuNode.h"
#include "UIElement/Menu/MenuEntry.h"
#include "UIElement/UIElementFactory.h"

#include <algorithm>
#include <sstream>

DynamicMenuRenderer::DynamicMenuRenderer()
  : lastElementCount(0)
{
}

DynamicMenuRenderer::~DynamicMenuRenderer() {}

void DynamicMenuRenderer::buildFrom(const std::vector<std::shared_ptr<UIElement>>& elements)
{
    rootNodes.clear();
    lastElementCount = elements.size();

    for (const auto& elem : elements)
    {
        auto menuEntry = std::dynamic_pointer_cast<MenuEntry>(elem);
        if (!menuEntry || menuEntry->getPath().empty())
        {
            continue;
        }

        auto tokens = splitPath(menuEntry->getPath());
        if (tokens.empty())
        {
            continue;
        }

        std::shared_ptr<DynamicMenuNode> root;
        for (const auto& r : rootNodes)
        {
            if (r && r->getName() == tokens[0])
            {
                root = r;
                break;
            }
        }
        if (!root)
        {
            root = std::make_shared<DynamicMenuNode>(tokens[0]);
            rootNodes.push_back(root);
        }

        auto current = root;
        for (size_t i = 1; i < tokens.size(); i++)
        {
            current = current->getOrCreateChild(tokens[i]);
        }
        current->setLeafEntry(menuEntry);
    }

    std::stable_sort(rootNodes.begin(), rootNodes.end(), [this](const auto& a, const auto& b) { return getMenuRank(a->getName()) < getMenuRank(b->getName()); });
}

void DynamicMenuRenderer::render(const UIElementFactory& factory)
{
    auto allElements = factory.getAllElements();
    if (allElements.size() != lastElementCount || rootNodes.empty())
    {
        buildFrom(allElements);
    }
    render();
}

void DynamicMenuRenderer::render()
{
    for (const auto& root : rootNodes)
    {
        if (root)
        {
            root->render();
        }
    }
}

const std::vector<std::shared_ptr<DynamicMenuNode>>& DynamicMenuRenderer::getRoots() const
{
    return rootNodes;
}

std::vector<std::string> DynamicMenuRenderer::splitPath(const std::string& path) const
{
    std::vector<std::string> tokens;
    std::stringstream        ss(path);
    std::string              token;

    while (std::getline(ss, token, '/'))
    {
        if (!token.empty())
        {
            tokens.push_back(token);
        }
    }
    return tokens;
}

int DynamicMenuRenderer::getMenuRank(const std::string& menuName) const
{
    if (menuName == "File")
    {
        return 0;
    }
    if (menuName == "Edit")
    {
        return 1;
    }
    if (menuName == "View")
    {
        return 2;
    }
    if (menuName == "Create")
    {
        return 3;
    }
    if (menuName == "Help")
    {
        return 100;
    }
    return 50;
}

#ifdef ISTESTPROJECT
#include "Execution/GatedDatabase.h"
#include "Execution/UndoStack.h"
#include "Windows/Windows.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("DynamicMenuRenderer/TreeBuilding")
{
    GatedDatabase    db;
    UndoStack        us(db);
    Windows          windows;
    bool             quitFlag = false;
    UIElementFactory factory;
    factory.registerDefaults(db, us, windows, quitFlag);

    DynamicMenuRenderer renderer;
    renderer.buildFrom(factory.getAllElements());

    const auto& roots = renderer.getRoots();
    REQUIRE(roots.size() == 4);
    REQUIRE(roots[0]->getName() == "File");
    REQUIRE(roots[1]->getName() == "Edit");
    REQUIRE(roots[2]->getName() == "Create");
    REQUIRE(roots[3]->getName() == "Help");

    REQUIRE(roots[0]->getChildren().size() == 3);
    REQUIRE(roots[0]->getChildren()[0]->getName() == "Load");
    REQUIRE(roots[0]->getChildren()[0]->getChildren().size() == 3);
    REQUIRE(roots[0]->getChildren()[1]->getName() == "Preferences");
    REQUIRE(roots[0]->getChildren()[2]->getName() == "Quit");
}

TEST_CASE("DynamicMenuRenderer/MenuRanking")
{
    DynamicMenuRenderer renderer;
    class DummyEntry : public MenuEntry
    {
      public:
        DummyEntry(const std::string& path, GatedDatabase& db, UndoStack& us, Windows& w)
          : MenuEntry("Test", path, "", db, us, w)
        {
        }
        virtual ~DummyEntry() {}
        virtual std::string getType() const override
        {
            return "Test";
        }
        virtual std::string getDescription() const override
        {
            return "";
        }
        virtual void click() override {}
    };

    GatedDatabase                           db;
    UndoStack                               us(db);
    Windows                                 windows;
    std::vector<std::shared_ptr<UIElement>> elements;
    elements.push_back(std::make_shared<DummyEntry>("View/Reset", db, us, windows));
    elements.push_back(std::make_shared<DummyEntry>("Help/About", db, us, windows));
    elements.push_back(std::make_shared<DummyEntry>("File/Open", db, us, windows));
    elements.push_back(std::make_shared<DummyEntry>("Create/Shape", db, us, windows));
    elements.push_back(std::make_shared<DummyEntry>("Edit/Undo", db, us, windows));

    renderer.buildFrom(elements);
    const auto& roots = renderer.getRoots();
    REQUIRE(roots.size() == 5);
    REQUIRE(roots[0]->getName() == "File");
    REQUIRE(roots[1]->getName() == "Edit");
    REQUIRE(roots[2]->getName() == "View");
    REQUIRE(roots[3]->getName() == "Create");
    REQUIRE(roots[4]->getName() == "Help");
}
#endif
