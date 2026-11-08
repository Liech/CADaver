#pragma once

#include <memory>
#include <string>
#include <vector>

class DynamicMenuNode;
class UIElement;
class UIElementFactory;

// Dynamically constructs and renders hierarchical top menu bars from registered MenuEntry items.
class DynamicMenuRenderer
{
  public:
    DynamicMenuRenderer();
    virtual ~DynamicMenuRenderer();

    void buildFrom(const std::vector<std::shared_ptr<UIElement>>& elements);
    void render(const UIElementFactory& factory);
    void render();

    const std::vector<std::shared_ptr<DynamicMenuNode>>& getRoots() const;

  private:
    std::vector<std::string> splitPath(const std::string& path) const;
    int                      getMenuRank(const std::string& menuName) const;

    std::vector<std::shared_ptr<DynamicMenuNode>> rootNodes;
    size_t                                        lastElementCount;
};
