#include "Windows/Conversion/SvoToStlConversionModal.h"

#include "Database/Asset/VoxelAsset.h"
#include "Database/Database.h"
#include "Database/Project.h"
#include "Execution/Defer.h"
#include "Execution/GatedDatabase.h"

#include <imgui.h>
#include <memory>

SvoToStlConversionModal::SvoToStlConversionModal()
  : assetComboBox("Voxel")
{
}

SvoToStlConversionModal::~SvoToStlConversionModal() {}

std::string SvoToStlConversionModal::getType() const
{
    return "SvoToStlConversionModal";
}

std::string SvoToStlConversionModal::getDescription() const
{
    return "Window: SvoToStlConversionModal\n"
           "Title: Convert to STL\n"
           "Purpose: Modal dialog for extracting a blocky boundary triangulation mesh asset from a Sparse Voxel Octree (SVO) volumetric asset at a chosen LOD depth.\n\n"
           "Parameters:\n"
           "  - Target Asset: Voxel asset from the project to mesh\n"
           "  - Target LOD Depth: Octree hierarchy depth level to extract (0 up to log2(dimension))\n"
           "  - Delete original SVO asset: Removes the source voxel asset upon successful conversion\n\n"
           "Dispatched Command: ConvertSvoToStl (async)";
}

bool SvoToStlConversionModal::isOpen() const
{
    return openState;
}

void SvoToStlConversionModal::open()
{
    deleteOrigin    = false;
    openState       = true;
    shouldOpenPopup = true;
}

void SvoToStlConversionModal::close()
{
    openState       = false;
    shouldOpenPopup = false;
}

void SvoToStlConversionModal::setTargetAsset(const CADacomb::GUID& assetGuid)
{
    assetComboBox.setSelectedGuid(assetGuid);
}

void SvoToStlConversionModal::render()
{
    if (!openState || !gatedDatabase)
    {
        return;
    }

    const auto& project = gatedDatabase->get().getProject();

    if (shouldOpenPopup)
    {
        ImGui::OpenPopup("Convert to STL##SvoModal");
        shouldOpenPopup = false;
        updateMaxDepthFromSelectedAsset(project, true);
    }

    if (ImGui::BeginPopupModal("Convert to STL##SvoModal", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        CADacomb::GUID previousGuid = assetComboBox.getSelectedGuid();
        bool           changed      = assetComboBox.render(project);
        if (changed || assetComboBox.getSelectedGuid() != previousGuid)
        {
            updateMaxDepthFromSelectedAsset(project, true);
        }
        else
        {
            updateMaxDepthFromSelectedAsset(project, false);
        }
        ImGui::Spacing();

        ImGui::SliderInt("Target LOD Depth", &targetDepth, 0, maxTreeDepth);

        ImGui::Checkbox("Delete original SVO asset", &deleteOrigin);
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        bool hasTarget = project.hasAsset(assetComboBox.getSelectedGuid());
        ImGui::BeginDisabled(!hasTarget);
        if (ImGui::Button("Convert", ImVec2(120.0f, 0.0f)))
        {
            executeConversion();
            close();
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndDisabled();

        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120.0f, 0.0f)))
        {
            close();
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
    else if (!shouldOpenPopup)
    {
        openState = false;
    }
}

void SvoToStlConversionModal::executeConversion()
{
    if (!gatedDatabase || !defer)
    {
        return;
    }

    GatedDatabase* db        = gatedDatabase;
    CADacomb::GUID guid      = assetComboBox.getSelectedGuid();
    int            depth     = targetDepth;
    bool           delOrigin = deleteOrigin;

    defer->defer(
      [db, guid, depth, delOrigin]()
      {
          db->executeAsync({
            {         "type", "ConvertSvoToStl" },
            {      "version",                 1 },
            {    "assetGuid",   guid.toString() },
            {  "targetDepth",             depth },
            { "deleteOrigin",         delOrigin }
          });
      });
}

void SvoToStlConversionModal::updateMaxDepthFromSelectedAsset(const Project& project, bool resetTargetToMax)
{
    auto asset      = project.getAsset(assetComboBox.getSelectedGuid());
    auto voxelAsset = std::dynamic_pointer_cast<const VoxelAsset>(asset);
    if (!voxelAsset)
    {
        return;
    }

    int      computedMax = 6;
    uint32_t dim         = voxelAsset->getSVO().getDimension();
    uint32_t d           = 0;
    while ((1u << d) < dim)
    {
        d++;
    }
    if (d > 0)
    {
        computedMax = static_cast<int>(d);
    }

    maxTreeDepth = computedMax;
    if (resetTargetToMax || targetDepth > maxTreeDepth || targetDepth < 0)
    {
        targetDepth = maxTreeDepth;
    }
}
