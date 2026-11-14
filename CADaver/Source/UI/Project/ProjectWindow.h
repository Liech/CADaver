#pragma once

#include <memory>

class AssetContextMenuRenderer;
class GatedDatabase;
class Windows;

// UI window displaying the project assets and providing context operations.
class ProjectWindow
{
  public:
    ProjectWindow();
    virtual ~ProjectWindow();

    void render(GatedDatabase& gatedDatabase, Windows& windows);

    float getWidth() const;
    void  setWidth(float width);

  private:
    void renderMenuBar(bool hasAssets, Windows& windows);

    std::unique_ptr<AssetContextMenuRenderer> contextMenuRenderer;
    float                                     width = 300.0f;
};
