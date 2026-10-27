#include "AssetContextMenuFactory.h"

#include "AssetContextMenu.h"
#include "CadAssetContextMenu.h"
#include "Commands/Defer.h"
#include "Commands/GatedDatabase.h"
#include "Database/Asset.h"
#include "Database/AssetType.h"
#include "TriangulationAssetContextMenu.h"
#include "UI/Converter/CadToStlConversionModal.h"
#include "UI/Converter/CadToSvoConversionModal.h"
#include "UI/Converter/StlToSvoConversionModal.h"
#include "UI/Converter/SvoToStlConversionModal.h"
#include "VoxelAssetContextMenu.h"

#include <cstdint>
#include <imgui.h>
#include <string>

AssetContextMenuFactory::AssetContextMenuFactory()
  : cadToStlModal(std::make_unique<CadToStlConversionModal>())
  , cadToSvoModal(std::make_unique<CadToSvoConversionModal>())
  , stlToSvoModal(std::make_unique<StlToSvoConversionModal>())
  , svoToStlModal(std::make_unique<SvoToStlConversionModal>())
{
}

AssetContextMenuFactory::~AssetContextMenuFactory() {}

std::unique_ptr<AssetContextMenu> AssetContextMenuFactory::create(const std::shared_ptr<Asset>& asset)
{
    if (!asset)
    {
        return nullptr;
    }

    if (asset->getType() == AssetType::Triangulation)
    {
        auto menu = std::make_unique<TriangulationAssetContextMenu>();
        menu->setStlToSvoModal(stlToSvoModal.get());
        return menu;
    }
    else if (asset->getType() == AssetType::Cad)
    {
        auto menu = std::make_unique<CadAssetContextMenu>();
        menu->setCadToStlModal(cadToStlModal.get());
        menu->setCadToSvoModal(cadToSvoModal.get());
        return menu;
    }
    else if (asset->getType() == AssetType::Voxel)
    {
        auto menu = std::make_unique<VoxelAssetContextMenu>();
        menu->setSvoToStlModal(svoToStlModal.get());
        return menu;
    }

    return nullptr;
}

void AssetContextMenuFactory::render(const std::shared_ptr<Asset>& asset)
{
    render(asset, -1);
}

void AssetContextMenuFactory::render(const std::shared_ptr<Asset>& asset, int assetIndex)
{
    if (!asset)
    {
        return;
    }

    std::string popupId = "AssetContextMenu##" + std::to_string(reinterpret_cast<uintptr_t>(asset.get()));
    if (ImGui::BeginPopupContextItem(popupId.c_str()))
    {
        auto menu = create(asset);
        if (menu)
        {
            menu->setAssetIndex(assetIndex);
            menu->render(*asset);
        }

        ImGui::Separator();
        if (ImGui::MenuItem("Close"))
        {
            closeTargetIndex = assetIndex;
            closeTargetName  = asset->getName();
            openCloseModal   = true;
        }

        ImGui::EndPopup();
    }
}

void AssetContextMenuFactory::renderCloseModal()
{
    if (openCloseModal)
    {
        ImGui::OpenPopup("Close Asset##Modal");
        openCloseModal = false;
    }

    if (ImGui::BeginPopupModal("Close Asset##Modal", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Are you sure you want to close '%s'?", closeTargetName.c_str());
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::Button("Yes", ImVec2(120.0f, 0.0f)))
        {
            int targetIndex = closeTargetIndex;
            Defer::getInstance().defer(
              [targetIndex]()
              {
                  GatedDatabase::getInstance().execute(
                    {
                      {       "type", "RemoveAsset" },
                      {    "version",             1 },
                      { "assetIndex",   targetIndex }
                  },
                    true);
              });
            ImGui::CloseCurrentPopup();
        }
        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120.0f, 0.0f)))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void AssetContextMenuFactory::renderConversionModals()
{
    if (cadToStlModal)
    {
        cadToStlModal->render();
    }
    if (cadToSvoModal)
    {
        cadToSvoModal->render();
    }
    if (stlToSvoModal)
    {
        stlToSvoModal->render();
    }
    if (svoToStlModal)
    {
        svoToStlModal->render();
    }
}
