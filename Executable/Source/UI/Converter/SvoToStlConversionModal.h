#pragma once

#include <string>

// Modal dialog for converting a volumetric SVO asset to a blocky triangulation STL asset.
class SvoToStlConversionModal
{
  public:
    SvoToStlConversionModal();
    virtual ~SvoToStlConversionModal();

    void open(int assetIndex, const std::string& assetName, int maxDepth = 6);
    void render();

  private:
    void executeConversion();

    std::string targetAssetName;
    int         targetAssetIndex = -1;
    int         targetDepth      = -1;
    int         maxTreeDepth     = 6;
    bool        deleteOrigin     = false;
    bool        shouldOpenPopup  = false;
};
