#pragma once

#include <string>

// Modal dialog for converting a CAD asset to an STL triangulation asset.
class CadToStlConversionModal
{
  public:
    CadToStlConversionModal();
    virtual ~CadToStlConversionModal();

    void open(int assetIndex, const std::string& assetName);
    void render();

  private:
    void executeConversion();

    std::string targetAssetName;
    int         targetAssetIndex = -1;
    double      precision        = 0.1;
    bool        deleteOrigin     = false;
    bool        shouldOpenPopup  = false;
};
