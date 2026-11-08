#pragma once

#include <memory>

class AssetContextMenuFactory;
class GatedDatabase;
class Project;
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
    void renderMenuBar(const Project& project, Windows& windows);

    std::unique_ptr<AssetContextMenuFactory> contextMenuFactory;
    float                                    width = 300.0f;
};
