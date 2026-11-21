#include "Windows/Conversion/SvoToStlSurfaceNetsConversionModal.h"

#include "Execution/Defer.h"
#include "Execution/GatedDatabase.h"

#include <imgui.h>

SvoToStlSurfaceNetsConversionModal::SvoToStlSurfaceNetsConversionModal()
  : assetComboBox("Voxel")
{
}

SvoToStlSurfaceNetsConversionModal::~SvoToStlSurfaceNetsConversionModal() {}

std::string SvoToStlSurfaceNetsConversionModal::getType() const
{
    return "SvoToStlSurfaceNetsConversion";
}

std::string SvoToStlSurfaceNetsConversionModal::getDescription() const
{
    return "Window: SvoToStlSurfaceNetsConversion\n"
           "Title: Convert to STL (SurfaceNets)\n"
           "Purpose: Dialog for extracting a smooth SurfaceNets triangulation mesh asset from a Sparse Voxel Octree (SVO) volumetric asset at a chosen LOD depth.\n\n"
           "Parameters:\n"
           "  - Target Asset: Voxel asset from the project to mesh\n"
           "  - Target LOD Depth: Octree hierarchy depth level to extract (0 up to log2(dimension))\n"
           "  - Delete original SVO asset: Removes the source voxel asset upon successful conversion\n\n"
           "Dispatched Command: ConvertSvoToStlSurfaceNets (async)";
}

void SvoToStlSurfaceNetsConversionModal::setTargetAsset(const CADacomb::GUID& assetGuid)
{
    assetComboBox.setSelectedGuid(assetGuid);
}

void SvoToStlSurfaceNetsConversionModal::render()
{
    if (!openState || !gatedDatabase)
    {
        return;
    }

    if (ImGui::Begin("Convert to STL (SurfaceNets)##SvoSurfaceNetsModal", &openState, ImGuiWindowFlags_AlwaysAutoResize))
    {
        bool changed = assetComboBox.render(*gatedDatabase);
        if (changed || assetComboBox.getSelectedGuid() != lastTrackedGuid)
        {
            lastTrackedGuid = assetComboBox.getSelectedGuid();
            updateMaxDepthFromSelectedAsset(true);
        }
        else
        {
            updateMaxDepthFromSelectedAsset(false);
        }
        ImGui::Spacing();

        ImGui::SliderInt("Target LOD Depth", &targetDepth, 0, maxTreeDepth);

        ImGui::Checkbox("Delete original SVO asset", &deleteOrigin);
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        bool hasTarget = !assetComboBox.getSelectedGuid().toString().empty();
        ImGui::BeginDisabled(!hasTarget);
        if (ImGui::Button("Convert", ImVec2(120.0f, 0.0f)))
        {
            executeConversion();
            close();
        }
        ImGui::EndDisabled();

        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120.0f, 0.0f)))
        {
            close();
        }
    }
    ImGui::End();
}

void SvoToStlSurfaceNetsConversionModal::executeConversion()
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
          Polyglot::Node command = {
              {         "type", "ConvertSvoToStlSurfaceNets" },
              {      "version",                            1 },
              {    "assetGuid",              guid.toString() },
              {  "targetDepth",                        depth },
              { "deleteOrigin",                    delOrigin }
          };
          db->executeAsync(command);
      });
}

void SvoToStlSurfaceNetsConversionModal::updateMaxDepthFromSelectedAsset(bool resetTargetToMax)
{
    if (!gatedDatabase || assetComboBox.getSelectedGuid().toString().empty())
    {
        return;
    }

    Polyglot::Node response = gatedDatabase->execute({
      {      "type",                             "GetAssetInfo" },
      {   "version",                                          1 },
      { "assetGuid", assetComboBox.getSelectedGuid().toString() }
    });
    nlohmann::json infoJson = response.toJson();
    if (infoJson.value("type", "") != "GetAssetInfo")
    {
        return;
    }

    uint32_t dim = infoJson.value("dimension", 0u);
    if (dim == 0)
    {
        return;
    }

    int      computedMax = 6;
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
