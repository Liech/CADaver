#pragma once

#include "CADacomb/Util/GUID.h"
#include "Component/AssetComboBox.h"
#include "Windows/Window.h"

#include <string>

// Non-modal window for converting a clustered Triangulation asset into a CAD asset.
class ConvertClusteringToCadWindow : public Window
{
  public:
    ConvertClusteringToCadWindow();
    virtual ~ConvertClusteringToCadWindow();

    std::string getType() const override;
    std::string getDescription() const override;
    void        render() override;

    void setTargetAsset(const CADacomb::GUID& assetGuid);
    void startDefault(const CADacomb::GUID& assetGuid);

  private:
    void renderComponentComboBox();
    void executeConvert();
    void createDefaultClustering();

    AssetComboBox  assetComboBox;
    CADacomb::GUID selectedComponentGuid;
    float          planarityTolerance        = 0.001f;
    float          boundarySimplifyTolerance = 0.0001f;
    float          stitchTolerance           = 0.001f;
    bool           tryMakeSolid              = true;
    bool           useDefaultClustering      = false;
};
