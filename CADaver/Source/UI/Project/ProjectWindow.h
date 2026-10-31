#pragma once

#include <memory>
#include <string>

class AssetContextMenuFactory;
class GatedDatabase;
class Project;

// UI window displaying the project assets and providing context operations.
class ProjectWindow
{
  public:
    ProjectWindow();
    virtual ~ProjectWindow();

    void render(GatedDatabase& gatedDatabase);

    float getWidth() const;
    void  setWidth(float width);

  private:
    void renderMenuBar(const Project& project);
    void renderClearModal(GatedDatabase& gatedDatabase);
    void renderErrorModal();

    std::unique_ptr<AssetContextMenuFactory> contextMenuFactory;
    std::string                              errorMessage;
    float                                    width          = 300.0f;
    bool                                     openClearModal = false;
    bool                                     openErrorModal = false;
};
