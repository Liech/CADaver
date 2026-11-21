#pragma once

#include "CADacomb/Util/GUID.h"

#include <glm/glm.hpp>
#include <memory>
#include <string>

class AssetContextMenuFactory;
class AssetGizmoController;
class GatedDatabase;
class Windows;

// Renders asset context menus inline or as named popups using AssetContextMenuFactory.
class AssetContextMenuRenderer
{
  public:
    AssetContextMenuRenderer();
    virtual ~AssetContextMenuRenderer();

    void render(GatedDatabase& gatedDatabase, Windows& windows, const CADacomb::GUID& assetGuid, const std::string& assetType);
    void renderPopup(GatedDatabase&        gatedDatabase,
                     Windows&              windows,
                     const CADacomb::GUID& assetGuid,
                     const std::string&    assetType,
                     const std::string&    popupId,
                     AssetGizmoController& gizmoController,
                     const glm::dvec3&     hitPoint);

  private:
    void renderMenuContents(GatedDatabase&        gatedDatabase,
                            Windows&              windows,
                            const CADacomb::GUID& assetGuid,
                            const std::string&    assetType,
                            AssetGizmoController* gizmoController,
                            const glm::dvec3&     hitPoint);

    std::unique_ptr<AssetContextMenuFactory> factory;
};
