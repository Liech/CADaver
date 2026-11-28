#include "Windows/Conversion/SdfToSvoConversionModal.h"

#include "Execution/Defer.h"
#include "Execution/GatedDatabase.h"

#include <imgui.h>

SdfToSvoConversionModal::SdfToSvoConversionModal()
  : assetComboBox("Sdf")
{
}

SdfToSvoConversionModal::~SdfToSvoConversionModal() {}

std::string SdfToSvoConversionModal::getType() const
{
    return "SdfToSvoConversion";
}

std::string SdfToSvoConversionModal::getDescription() const
{
    return "Window: SdfToSvoConversion\n"
           "Title: Convert SDF to SVO\n"
           "Purpose: Dialog for voxelizing an SdfAsset into a Sparse Voxel Octree (SVO) volumetric asset.\n\n"
           "Parameters:\n"
           "  - Target Asset: SDF asset from the project to voxelize\n"
           "  - Voxel Resolution: Cubic SVO grid resolution (16, 32, 64, 128, 256, 512, default: 64)\n"
           "  - Delete original SDF asset: Removes the source SDF asset upon successful conversion\n\n"
           "Dispatched Command: ConvertSdfToSvo (async)";
}

void SdfToSvoConversionModal::setTargetAsset(const CADacomb::GUID& assetGuid)
{
    assetComboBox.setSelectedGuid(assetGuid);
}

void SdfToSvoConversionModal::render()
{
    if (!openState || !gatedDatabase)
    {
        return;
    }

    if (ImGui::Begin("Convert SDF to SVO##SdfToSvoModal", &openState, ImGuiWindowFlags_AlwaysAutoResize))
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

void SdfToSvoConversionModal::executeConversion()
{
    if (!gatedDatabase || !defer)
    {
        return;
    }

    GatedDatabase* db        = gatedDatabase;
    CADacomb::GUID guid      = assetComboBox.getSelectedGuid();
    uint32_t       dim       = static_cast<uint32_t>(resolutionComboBox.getResolution());
    bool           delOrigin = deleteOrigin;

    defer->defer(
      [db, guid, dim, delOrigin]()
      {
          Polyglot::Node command = {
              {         "type", "ConvertSdfToSvo" },
              {      "version",                 1 },
              {    "assetGuid",   guid.toString() },
              {    "dimension",               dim },
              { "deleteOrigin",         delOrigin }
          };
          db->executeAsync(command);
      });
}
