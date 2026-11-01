#pragma once

#include <memory>
#include <string>
#include <vector>

class MenuEntry;

// Represents a node in the hierarchical main menu tree structure.
class DynamicMenuNode
{
  public:
    explicit DynamicMenuNode(const std::string& name);
    virtual ~DynamicMenuNode();

    const std::string&                                   getName() const;
    std::shared_ptr<MenuEntry>                           getLeafEntry() const;
    void                                                 setLeafEntry(std::shared_ptr<MenuEntry> entry);
    const std::vector<std::shared_ptr<DynamicMenuNode>>& getChildren() const;
    std::shared_ptr<DynamicMenuNode>                     getOrCreateChild(const std::string& childName);

    void render();

  private:
    std::string                                   name;
    std::shared_ptr<MenuEntry>                    leafEntry;
    std::vector<std::shared_ptr<DynamicMenuNode>> children;
};
