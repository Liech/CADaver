#include "Windows/Conversion/CadToSvoConversionModal.h"

#include "Database/Database.h"
#include "Database/Project.h"
#include "Execution/Defer.h"
#include "Execution/GatedDatabase.h"

#include <imgui.h>

CadToSvoConversionModal::CadToSvoConversionModal()
  : assetComboBox("Cad")
{
}

CadToSvoConversionModal::~CadToSvoConversionModal() {}

std::string CadToSvoConversionModal::getType() const
{
    return "CadToSvoConversionModal";
}

std::string CadToSvoConversionModal::getDescription() const
{
    return "Window: CadToSvoConversionModal\n"
           "Title: Convert to SVO\n"
           "Purpose: Modal dialog for converting a STEP/CAD asset directly into a Sparse Voxel Octree (SVO) volumetric asset via intermediate triangulation.\n\n"
           "Parameters:\n"
           "  - Target Asset: CAD asset from the project to convert\n"
           "  - Voxel Resolution: Cubic SVO grid resolution (16, 32, 64, 128, 256, 512, default: 64)\n"
           "  - Tessellation Precision: Linear deflection tolerance for intermediate triangulation (0.001 to 1.0, default: 0.1)\n"
           "  - Delete original CAD asset: Removes the source CAD asset upon successful conversion\n\n"
           "Dispatched Command: ConvertCadToSvo (async)";
}

bool CadToSvoConversionModal::isOpen() const
{
    return openState;
}

void CadToSvoConversionModal::open()
{
    precision       = 0.1;
    deleteOrigin    = false;
    openState       = true;
    shouldOpenPopup = true;
}

void CadToSvoConversionModal::close()
{
    openState       = false;
    shouldOpenPopup = false;
}

void CadToSvoConversionModal::setTargetAsset(const CADacomb::GUID& assetGuid)
{
    assetComboBox.setSelectedGuid(assetGuid);
}

void CadToSvoConversionModal::render()
{
    if (!openState || !gatedDatabase)
    {
        return;
    }

    if (shouldOpenPopup)
    {
        ImGui::OpenPopup("Convert to SVO##CadModal");
        shouldOpenPopup = false;
    }

    if (ImGui::BeginPopupModal("Convert to SVO##CadModal", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        const auto& project = gatedDatabase->get().getProject();
        assetComboBox.render(project);
        ImGui::Spacing();

        resolutionComboBox.render();

        float p = static_cast<float>(precision);
        if (ImGui::SliderFloat("Tessellation Precision", &p, 0.001f, 1.0f, "%.3f", ImGuiSliderFlags_Logarithmic))
        {
            precision = static_cast<double>(p);
        }

        ImGui::Checkbox("Delete original CAD asset", &deleteOrigin);
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

void CadToSvoConversionModal::executeConversion()
{
    if (!gatedDatabase || !defer)
    {
        return;
    }

    GatedDatabase* db        = gatedDatabase;
    CADacomb::GUID guid      = assetComboBox.getSelectedGuid();
    double         prec      = precision;
    uint32_t       dim       = static_cast<uint32_t>(resolutionComboBox.getResolution());
    bool           delOrigin = deleteOrigin;

    defer->defer(
      [db, guid, prec, dim, delOrigin]()
      {
          db->executeAsync({
            {         "type", "ConvertCadToSvo" },
            {      "version",                 1 },
            {    "assetGuid",   guid.toString() },
            {    "precision",              prec },
            {    "dimension",               dim },
            { "deleteOrigin",         delOrigin }
          });
      });
}
