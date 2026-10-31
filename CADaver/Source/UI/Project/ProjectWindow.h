#pragma once

#include <memory>
#include <string>

class Project;
class AssetContextMenuFactory;

// UI window displaying the project assets and providing context operations.
class ProjectWindow
{
  public:
    ProjectWindow();
    virtual ~ProjectWindow();

    void render(const Project& project);

    float getWidth() const;
    void  setWidth(float width);

  private:
    void renderMenuBar(const Project& project);
    void renderClearModal();
    void renderErrorModal();

    std::unique_ptr<AssetContextMenuFactory> contextMenuFactory;
    std::string                              errorMessage;
    float                                    width          = 300.0f;
    bool                                     openClearModal = false;
    bool                                     openErrorModal = false;
};
