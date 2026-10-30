#pragma once

#include "CADacomb/Util/GUID.h"

#include <string>

// Modal dialog for converting a volumetric SVO asset to a blocky triangulation STL asset.
class SvoToStlConversionModal
{
  public:
    SvoToStlConversionModal();
    virtual ~SvoToStlConversionModal();

    void open(const CADacomb::GUID& assetGuid, const std::string& assetName, int maxDepth = 6);
    void render();

  private:
    void executeConversion();

    std::string    targetAssetName;
    CADacomb::GUID targetAssetGuid;
    int            targetDepth     = -1;
    int            maxTreeDepth    = 6;
    bool           deleteOrigin    = false;
    bool           shouldOpenPopup = false;
};
