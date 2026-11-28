#include "Windows/Conversion/SdfToStlConversionModal.h"

#include "Execution/Defer.h"
#include "Execution/GatedDatabase.h"

#include <imgui.h>

SdfToStlConversionModal::SdfToStlConversionModal()
  : assetComboBox("Sdf")
{
}

SdfToStlConversionModal::~SdfToStlConversionModal() {}

std::string SdfToStlConversionModal::getType() const
{
    return "SdfToStlConversion";
}

std::string SdfToStlConversionModal::getDescription() const
{
    return "Window: SdfToStlConversion\n"
           "Title: Convert SDF to STL (SurfaceNets)\n"
           "Purpose: Dialog for meshing an SdfAsset into a TriangulationAsset using SurfaceNets.\n\n"
           "Parameters:\n"
           "  - Target Asset: SDF asset from the project to mesh\n"
           "  - Grid Resolution: Sampling grid resolution (16, 32, 64, 128, 256, 512, default: 64)\n"
           "  - Delete original SDF asset: Removes the source SDF asset upon successful conversion\n\n"
           "Dispatched Command: ConvertSdfToStl (async)";
}

void SdfToStlConversionModal::setTargetAsset(const CADacomb::GUID& assetGuid)
{
    assetComboBox.setSelectedGuid(assetGuid);
}

void SdfToStlConversionModal::render()
{
    if (!openState || !gatedDatabase)
    {
        return;
    }

    if (ImGui::Begin("Convert SDF to STL##SdfToStlModal", &openState, ImGuiWindowFlags_AlwaysAutoResize))
    {
        assetComboBox.render(*gatedDatabase);
        ImGui::Spacing();

        resolutionComboBox.render();

        ImGui::Checkbox("Delete original SDF asset", &deleteOrigin);
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

void SdfToStlConversionModal::executeConversion()
{
    if (!gatedDatabase || !defer)
    {
        return;
    }

    GatedDatabase* db         = gatedDatabase;
    CADacomb::GUID guid       = assetComboBox.getSelectedGuid();
    uint32_t       resolution = static_cast<uint32_t>(resolutionComboBox.getResolution());
    bool           delOrigin  = deleteOrigin;

    defer->defer(
      [db, guid, resolution, delOrigin]()
      {
          Polyglot::Node command = {
              {         "type", "ConvertSdfToStl" },
              {      "version",                 1 },
              {    "assetGuid",   guid.toString() },
              {   "resolution",        resolution },
              { "deleteOrigin",         delOrigin }
          };
          db->executeAsync(command);
      });
}
