#pragma once

#include "CADacomb/Util/GUID.h"

#include <memory>
#include <string>

class Project;
class AssetContextMenuFactory;

// UI window displaying the project assets and providing selection and context operations.
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
    CADacomb::GUID                           selectedAssetGuid;
    float                                    width          = 300.0f;
    bool                                     openClearModal = false;
    bool                                     openErrorModal = false;
};
