#include "UI/Converter/CadToStlConversionModal.h"

#include "Execution/Defer.h"
#include "Execution/GatedDatabase.h"

#include <imgui.h>

CadToStlConversionModal::CadToStlConversionModal() {}

CadToStlConversionModal::~CadToStlConversionModal() {}

void CadToStlConversionModal::open(const CADacomb::GUID& assetGuid, const std::string& assetName)
{
    targetAssetGuid = assetGuid;
    targetAssetName = assetName;
    precision       = 0.1;
    deleteOrigin    = false;
    shouldOpenPopup = true;
}

void CadToStlConversionModal::render(GatedDatabase& gatedDatabase)
{
    if (shouldOpenPopup)
    {
        ImGui::OpenPopup("Convert to STL##Modal");
        shouldOpenPopup = false;
    }

    if (ImGui::BeginPopupModal("Convert to STL##Modal", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Converting: %s", targetAssetName.c_str());
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

        if (ImGui::Button("Convert", ImVec2(120.0f, 0.0f)))
        {
            executeConversion(gatedDatabase);
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

void CadToStlConversionModal::executeConversion(GatedDatabase& gatedDatabase)
{
    CADacomb::GUID guid      = targetAssetGuid;
    double         prec      = precision;
    bool           delOrigin = deleteOrigin;

    Defer::getInstance().defer(
      [&gatedDatabase, guid, prec, delOrigin]()
      {
          gatedDatabase.executeAsync({
            {         "type", "ConvertCadToStl" },
            {      "version",                 1 },
            {    "assetGuid",   guid.toString() },
            {    "precision",              prec },
            { "deleteOrigin",         delOrigin }
          });
      });
}
