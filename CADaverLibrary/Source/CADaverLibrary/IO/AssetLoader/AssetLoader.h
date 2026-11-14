#pragma once

#include "Polyglot/Node.h"

#include <memory>
#include <string>

class Asset;

// Abstract base class for loading a specific asset type from a Polyglot source node.
class AssetLoader
{
  public:
    AssetLoader();
    virtual ~AssetLoader();

    virtual std::shared_ptr<Asset> load(const std::string& name, const Polyglot::Node& source) const = 0;
};
