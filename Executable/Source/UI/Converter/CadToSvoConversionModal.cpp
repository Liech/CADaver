#include "CadToSvoConversionModal.h"

#include "Commands/Defer.h"
#include "Commands/GatedDatabase.h"

#include <imgui.h>

CadToSvoConversionModal::CadToSvoConversionModal() {}

CadToSvoConversionModal::~CadToSvoConversionModal() {}

void CadToSvoConversionModal::open(int assetIndex, const std::string& assetName)
{
    targetAssetIndex = assetIndex;
    targetAssetName  = assetName;
    precision        = 0.1;
    deleteOrigin     = false;
    resolutionComboBox.setResolution(64);
    shouldOpenPopup = true;
}

void CadToSvoConversionModal::render()
{
    if (shouldOpenPopup)
    {
        ImGui::OpenPopup("Convert CAD to SVO##Modal");
        shouldOpenPopup = false;
    }

    if (ImGui::BeginPopupModal("Convert CAD to SVO##Modal", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Convert CAD '%s' to SVO", targetAssetName.c_str());
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        float prec = static_cast<float>(precision);
        if (ImGui::SliderFloat("Deflection / Precision", &prec, 0.001f, 1.0f, "%.4f", ImGuiSliderFlags_Logarithmic))
        {
            precision = static_cast<double>(prec);
        }
        ImGui::TextDisabled("Controls intermediate triangulation chordal deflection.");

        ImGui::Spacing();
        resolutionComboBox.render();

        ImGui::Spacing();
        ImGui::Checkbox("Delete original asset", &deleteOrigin);

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::Button("Convert", ImVec2(120.0f, 0.0f)))
        {
            executeConversion();
            ImGui::CloseCurrentPopup();
        }

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
    int index = targetAssetIndex;
    if (index < 0)
    {
        return;
    }

    double   prec         = precision;
    uint32_t dim          = resolutionComboBox.getResolution();
    bool     shouldDelete = deleteOrigin;
    Defer::getInstance().defer(
      [index, prec, dim, shouldDelete]()
      {
          GatedDatabase::getInstance().execute(
            {
              {         "type", "ConvertCadToSvo" },
              {      "version",                 1 },
              {   "assetIndex",             index },
              {    "precision",              prec },
              {    "dimension",               dim },
              { "deleteOrigin",      shouldDelete }
          },
            true);
      });
}
