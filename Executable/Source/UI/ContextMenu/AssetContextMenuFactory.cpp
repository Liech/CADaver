#include "UI/ContextMenu/AssetContextMenuFactory.h"

#include "Commands/Defer.h"
#include "Commands/GatedDatabase.h"
#include "Database/Asset/CadAsset.h"
#include "Database/Asset/TriangulationAsset.h"
#include "Database/Asset/VoxelAsset.h"
#include "UI/ContextMenu/CadAssetContextMenu.h"
#include "UI/ContextMenu/TriangulationAssetContextMenu.h"
#include "UI/ContextMenu/VoxelAssetContextMenu.h"
#include "UI/Converter/CadToStlConversionModal.h"
#include "UI/Converter/CadToSvoConversionModal.h"
#include "UI/Converter/StlToSvoConversionModal.h"
#include "UI/Converter/SvoToStlConversionModal.h"

#include <imgui.h>

AssetContextMenuFactory::AssetContextMenuFactory()
{
    cadToStlModal = std::make_unique<CadToStlConversionModal>();
    cadToSvoModal = std::make_unique<CadToSvoConversionModal>();
    stlToSvoModal = std::make_unique<StlToSvoConversionModal>();
    svoToStlModal = std::make_unique<SvoToStlConversionModal>();
}

AssetContextMenuFactory::~AssetContextMenuFactory() {}

std::unique_ptr<AssetContextMenu> AssetContextMenuFactory::create(const std::shared_ptr<Asset>& asset)
{
    if (!asset)
    {
        return nullptr;
    }

    if (asset->getType() == "Cad")
    {
        auto menu = std::make_unique<CadAssetContextMenu>();
        menu->setCadToStlModal(cadToStlModal.get());
        menu->setCadToSvoModal(cadToSvoModal.get());
        return menu;
    }
    else if (asset->getType() == "Triangulation")
    {
        auto menu = std::make_unique<TriangulationAssetContextMenu>();
        menu->setStlToSvoModal(stlToSvoModal.get());
        return menu;
    }
    else if (asset->getType() == "Voxel")
    {
        auto menu = std::make_unique<VoxelAssetContextMenu>();
        menu->setSvoToStlModal(svoToStlModal.get());
        return menu;
    }

    return nullptr;
}

void AssetContextMenuFactory::render(const std::shared_ptr<Asset>& asset)
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
            menu->setAssetGuid(asset->getGuid());
            menu->render(*asset);
        }

        ImGui::Separator();
        if (ImGui::MenuItem("Close"))
        {
            closeTargetGuid = asset->getGuid();
            closeTargetName = asset->getName();
            openCloseModal  = true;
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
            CADacomb::GUID targetGuid = closeTargetGuid;
            Defer::getInstance().defer(
              [targetGuid]()
              {
                  GatedDatabase::getInstance().execute(
                    {
                      {      "type",         "RemoveAsset" },
                      {   "version",                     1 },
                      { "assetGuid", targetGuid.toString() }
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
