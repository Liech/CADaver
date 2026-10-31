#pragma once

#include "CADacomb/Util/GUID.h"

#include <string>

class GatedDatabase;

// Modal dialog for converting a CAD asset to an STL triangulation asset.
class CadToStlConversionModal
{
  public:
    CadToStlConversionModal();
    virtual ~CadToStlConversionModal();

    void open(const CADacomb::GUID& assetGuid, const std::string& assetName);
    void render(GatedDatabase& gatedDatabase);

  private:
    void executeConversion(GatedDatabase& gatedDatabase);

    std::string    targetAssetName;
    CADacomb::GUID targetAssetGuid;
    double         precision       = 0.1;
    bool           deleteOrigin    = false;
    bool           shouldOpenPopup = false;
};
