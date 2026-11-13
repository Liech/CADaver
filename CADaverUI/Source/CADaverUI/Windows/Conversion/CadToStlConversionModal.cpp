#include "Windows/Conversion/CadToStlConversionModal.h"

#include "Execution/Defer.h"
#include "Execution/GatedDatabase.h"

#include <imgui.h>

CadToStlConversionModal::CadToStlConversionModal()
  : assetComboBox("Cad")
{
}

CadToStlConversionModal::~CadToStlConversionModal() {}

std::string CadToStlConversionModal::getType() const
{
    return "CadToStlConversion";
}

std::string CadToStlConversionModal::getDescription() const
{
    return "Window: CadToStlConversion\n"
           "Title: Convert to STL\n"
           "Purpose: Dialog for tessellating a STEP/CAD boundary-representation asset into a triangulated STL mesh asset.\n\n"
           "Parameters:\n"
           "  - Target Asset: CAD asset from the project to convert\n"
           "  - Tessellation Precision: Linear deflection tolerance for surface triangulation (0.001 to 1.0, default: 0.1)\n"
           "  - Delete original CAD asset: Removes the source CAD asset upon successful conversion\n\n"
           "Dispatched Command: ConvertCadToStl (async)";
}

void CadToStlConversionModal::setTargetAsset(const CADacomb::GUID& assetGuid)
{
    assetComboBox.setSelectedGuid(assetGuid);
}

void CadToStlConversionModal::render()
{
    if (!openState || !gatedDatabase)
    {
        return;
    }

    if (ImGui::Begin("Convert to STL##CadModal", &openState, ImGuiWindowFlags_AlwaysAutoResize))
    {
        assetComboBox.render(*gatedDatabase);
        ImGui::Spacing();

        float p = static_cast<float>(precision);
        if (ImGui::SliderFloat("Tessellation Precision", &p, 0.001f, 1.0f, "%.3f", ImGuiSliderFlags_Logarithmic))
        {
            precision = static_cast<double>(p);
        }

        ImGui::Checkbox("Delete original CAD asset", &deleteOrigin);
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

void CadToStlConversionModal::executeConversion()
{
    if (!gatedDatabase || !defer)
    {
        return;
    }

    GatedDatabase* db        = gatedDatabase;
    CADacomb::GUID guid      = assetComboBox.getSelectedGuid();
    double         prec      = precision;
    bool           delOrigin = deleteOrigin;

    defer->defer(
      [db, guid, prec, delOrigin]()
      {
          Polyglot::Node command = {
              {         "type", "ConvertCadToStl" },
              {      "version",                 1 },
              {    "assetGuid",   guid.toString() },
              {    "precision",              prec },
              { "deleteOrigin",         delOrigin }
          };
          db->executeAsync(command);
      });
}
