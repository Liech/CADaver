#include "UIElement/Menu/DynamicMenuNode.h"

#include "UIElement/Menu/MenuEntry.h"

#include <imgui.h>

DynamicMenuNode::DynamicMenuNode(const std::string& name)
  : name(name)
{
}

DynamicMenuNode::~DynamicMenuNode() {}

const std::string& DynamicMenuNode::getName() const
{
    return name;
}

std::shared_ptr<MenuEntry> DynamicMenuNode::getLeafEntry() const
{
    return leafEntry;
}

void DynamicMenuNode::setLeafEntry(std::shared_ptr<MenuEntry> entry)
{
    leafEntry = std::move(entry);
}

const std::vector<std::shared_ptr<DynamicMenuNode>>& DynamicMenuNode::getChildren() const
{
    return children;
}

std::shared_ptr<DynamicMenuNode> DynamicMenuNode::getOrCreateChild(const std::string& childName)
{
    for (const auto& child : children)
    {
        if (child && child->getName() == childName)
        {
            return child;
        }
    }

    auto newChild = std::make_shared<DynamicMenuNode>(childName);
    children.push_back(newChild);
    return newChild;
}

void DynamicMenuNode::render()
{
    if (leafEntry)
    {
        leafEntry->render();
        return;
    }

    if (ImGui::BeginMenu(name.c_str()))
    {
        for (const auto& child : children)
        {
            if (child)
            {
                if (child->getLeafEntry() && child->getLeafEntry()->hasSeparatorBefore())
                {
                    ImGui::Separator();
                }
                child->render();
            }
        }
        ImGui::EndMenu();
    }
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("DynamicMenuNode/Hierarchy")
{
    DynamicMenuNode root("Root");
    auto            childA  = root.getOrCreateChild("ChildA");
    auto            childA2 = root.getOrCreateChild("ChildA");
    auto            childB  = root.getOrCreateChild("ChildB");

    REQUIRE(childA == childA2);
    REQUIRE(root.getChildren().size() == 2);
    REQUIRE(childA->getName() == "ChildA");
    REQUIRE(childB->getName() == "ChildB");
}
#endif
