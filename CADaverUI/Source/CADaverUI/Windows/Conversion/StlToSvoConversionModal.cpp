#include "Windows/Conversion/StlToSvoConversionModal.h"

#include "Database/Database.h"
#include "Database/Project.h"
#include "Execution/Defer.h"
#include "Execution/GatedDatabase.h"

#include <imgui.h>

StlToSvoConversionModal::StlToSvoConversionModal()
  : assetComboBox("Triangulation")
{
}

StlToSvoConversionModal::~StlToSvoConversionModal() {}

std::string StlToSvoConversionModal::getType() const
{
    return "StlToSvoConversion";
}

std::string StlToSvoConversionModal::getDescription() const
{
    return "Window: StlToSvoConversion\n"
           "Title: Convert to SVO\n"
           "Purpose: Dialog for voxelizing a triangulated STL mesh asset into a Sparse Voxel Octree (SVO) volumetric asset.\n\n"
           "Parameters:\n"
           "  - Target Asset: Triangulation asset from the project to voxelize\n"
           "  - Voxel Resolution: Cubic SVO grid resolution (16, 32, 64, 128, 256, 512, default: 64)\n"
           "  - Delete original STL asset: Removes the source triangulation asset upon successful conversion\n\n"
           "Dispatched Command: ConvertStlToSvo (async)";
}

void StlToSvoConversionModal::setTargetAsset(const CADacomb::GUID& assetGuid)
{
    assetComboBox.setSelectedGuid(assetGuid);
}

void StlToSvoConversionModal::render()
{
    if (!openState || !gatedDatabase)
    {
        return;
    }

    if (ImGui::Begin("Convert to SVO##StlModal", &openState, ImGuiWindowFlags_AlwaysAutoResize))
    {
        const auto& project = gatedDatabase->get().getProject();
        assetComboBox.render(project);
        ImGui::Spacing();

        resolutionComboBox.render();

        ImGui::Checkbox("Delete original STL asset", &deleteOrigin);
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        bool hasTarget = project.hasAsset(assetComboBox.getSelectedGuid());
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

void StlToSvoConversionModal::executeConversion()
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
              {         "type", "ConvertStlToSvo" },
              {      "version",                 1 },
              {    "assetGuid",   guid.toString() },
              {    "dimension",               dim },
              { "deleteOrigin",         delOrigin }
          };
          db->executeAsync(command);
      });
}
