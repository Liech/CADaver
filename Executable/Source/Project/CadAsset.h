#pragma once

#include "Asset.h"

#include <string>

class CadAsset : public Asset
{
  public:
    CadAsset(const std::string& name);
    virtual ~CadAsset();
};
