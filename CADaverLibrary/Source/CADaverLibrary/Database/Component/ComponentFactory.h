#pragma once

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

class AssetComponent;

// Factory creating AssetComponent instances by component type name.
class ComponentFactory
{
  public:
    ComponentFactory();
    virtual ~ComponentFactory();

    std::shared_ptr<AssetComponent> create(const std::string& type) const;
    std::vector<std::string>        getRegisteredTypes() const;

  private:
    void registerComponent(const std::string& type, std::function<std::shared_ptr<AssetComponent>()> creator);
    void registerComponents();

    std::map<std::string, std::function<std::shared_ptr<AssetComponent>()>> creators;
};
