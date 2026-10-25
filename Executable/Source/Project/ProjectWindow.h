#pragma once

#include <memory>

class Project;
class AssetContextMenuFactory;

// UI window displaying the project assets and providing selection and context operations.
class ProjectWindow
{
  public:
    ProjectWindow();
    virtual ~ProjectWindow();

    void render(Project& project);

    float getWidth() const;
    void  setWidth(float width);

  private:
    std::unique_ptr<AssetContextMenuFactory> contextMenuFactory;
    float                                    width = 300.0f;
};
