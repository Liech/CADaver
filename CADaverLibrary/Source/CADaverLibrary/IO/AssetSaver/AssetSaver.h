#pragma once

#include "Polyglot/Node.h"

class Asset;

// Abstract base class for saving a specific asset type to a Polyglot destination node.
class AssetSaver
{
  public:
    AssetSaver();
    virtual ~AssetSaver();

    virtual bool save(const Asset& asset, const Polyglot::Node& destination) const = 0;
};
