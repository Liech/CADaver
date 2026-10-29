#pragma once

#include "CADacomb/Util/GUID.h"

#include <memory>
#include <string>

class Asset;
class AssetContextMenu;
class CadToStlConversionModal;
class CadToSvoConversionModal;
class StlToSvoConversionModal;
class SvoToStlConversionModal;

// Factory for creating and rendering asset context menus based on asset type.
class AssetContextMenuFactory
{
  public:
    AssetContextMenuFactory();
    virtual ~AssetContextMenuFactory();

    std::unique_ptr<AssetContextMenu> create(const std::shared_ptr<Asset>& asset);

    void render(const std::shared_ptr<Asset>& asset);
    void renderCloseModal();
    void renderConversionModals();

  private:
    std::unique_ptr<CadToStlConversionModal> cadToStlModal;
    std::unique_ptr<CadToSvoConversionModal> cadToSvoModal;
    std::unique_ptr<StlToSvoConversionModal> stlToSvoModal;
    std::unique_ptr<SvoToStlConversionModal> svoToStlModal;
    std::string                              closeTargetName;
    CADacomb::GUID                           closeTargetGuid;
    bool                                     openCloseModal = false;
};
