#include "UI/Converter/CadToSvoConversionModal.h"

#include "Commands/Defer.h"
#include "Commands/GatedDatabase.h"

#include <imgui.h>

CadToSvoConversionModal::CadToSvoConversionModal() {}

CadToSvoConversionModal::~CadToSvoConversionModal() {}

void CadToSvoConversionModal::open(const CADacomb::GUID& assetGuid, const std::string& assetName)
{
    targetAssetGuid = assetGuid;
    targetAssetName = assetName;
    precision       = 0.1;
    deleteOrigin    = false;
    shouldOpenPopup = true;
}

void CadToSvoConversionModal::render()
{
    if (shouldOpenPopup)
    {
        ImGui::OpenPopup("Convert to SVO##CadModal");
        shouldOpenPopup = false;
    }

    if (ImGui::BeginPopupModal("Convert to SVO##CadModal", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Converting: %s", targetAssetName.c_str());
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

        if (ImGui::Button("Convert", ImVec2(120.0f, 0.0f)))
        {
            executeConversion();
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

void CadToSvoConversionModal::executeConversion()
{
    CADacomb::GUID guid      = targetAssetGuid;
    double         prec      = precision;
    uint32_t       dim       = static_cast<uint32_t>(resolutionComboBox.getResolution());
    bool           delOrigin = deleteOrigin;

    Defer::getInstance().defer(
      [guid, prec, dim, delOrigin]()
      {
          GatedDatabase::getInstance().execute(
            {
              {         "type", "ConvertCadToSvo" },
              {      "version",                 1 },
              {    "assetGuid",   guid.toString() },
              {    "precision",              prec },
              {    "dimension",               dim },
              { "deleteOrigin",         delOrigin }
          },
            true);
      });
}
