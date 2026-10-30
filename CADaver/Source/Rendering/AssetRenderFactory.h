#pragma once

#include <functional>
#include <map>
#include <memory>
#include <string>

class AssetRenderer;

// Factory responsible for instantiating concrete AssetRenderer objects.
class AssetRenderFactory
{
  public:
    AssetRenderFactory();
    virtual ~AssetRenderFactory();

    static AssetRenderFactory& getInstance();

    std::shared_ptr<AssetRenderer> create(const std::string& type) const;

    void registerRenderer(const std::string& type, std::function<std::shared_ptr<AssetRenderer>()> creator);

  private:
    void registerRenderers();

    std::map<std::string, std::function<std::shared_ptr<AssetRenderer>()>> renderMap;
};
