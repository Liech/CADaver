#include "CadToStlConversionModal.h"

#include "Commands/Defer.h"
#include "Commands/GatedDatabase.h"

#include <imgui.h>

CadToStlConversionModal::CadToStlConversionModal() {}

CadToStlConversionModal::~CadToStlConversionModal() {}

void CadToStlConversionModal::open(int assetIndex, const std::string& assetName)
{
    targetAssetIndex = assetIndex;
    targetAssetName  = assetName;
    precision        = 0.1;
    deleteOrigin     = false;
    shouldOpenPopup  = true;
}

void CadToStlConversionModal::render()
{
    if (shouldOpenPopup)
    {
        ImGui::OpenPopup("Convert CAD to STL##Modal");
        shouldOpenPopup = false;
    }

    if (ImGui::BeginPopupModal("Convert CAD to STL##Modal", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Convert CAD '%s' to STL", targetAssetName.c_str());
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        float prec = static_cast<float>(precision);
        if (ImGui::SliderFloat("Deflection / Precision", &prec, 0.001f, 1.0f, "%.4f", ImGuiSliderFlags_Logarithmic))
        {
            precision = static_cast<double>(prec);
        }
        ImGui::TextDisabled("Controls triangulation chordal deflection.");

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

void CadToStlConversionModal::executeConversion()
{
    int index = targetAssetIndex;
    if (index < 0)
    {
        return;
    }

    double prec         = precision;
    bool   shouldDelete = deleteOrigin;
    Defer::getInstance().defer(
      [index, prec, shouldDelete]()
      {
          GatedDatabase::getInstance().execute(
            {
              {         "type", "ConvertCadToStl" },
              {      "version",                 1 },
              {   "assetIndex",             index },
              {    "precision",              prec },
              { "deleteOrigin",      shouldDelete }
          },
            true);
      });
}
