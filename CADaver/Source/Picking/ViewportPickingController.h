#pragma once

#include "CADacomb/Util/GUID.h"
#include "Vulcant/Toolkit/SceneData.h"

#include <memory>
#include <string>

class AssetContextMenuRenderer;
class GatedDatabase;
class Windows;

// Handles 3D viewport mouse picking, selection updates, and viewport asset context menus via commands.
class ViewportPickingController
{
  public:
    ViewportPickingController();
    virtual ~ViewportPickingController();

    void update(GatedDatabase& gatedDatabase, Windows& windows, const Vulcant::Toolkit::SceneData& sceneData);

  private:
    std::unique_ptr<AssetContextMenuRenderer> contextMenuRenderer;
    CADacomb::GUID                            contextMenuGuid;
    std::string                               contextMenuType;
};
