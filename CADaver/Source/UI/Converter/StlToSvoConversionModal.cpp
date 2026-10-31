#include "UI/Converter/StlToSvoConversionModal.h"

#include "Execution/Defer.h"
#include "Execution/GatedDatabase.h"

#include <imgui.h>

StlToSvoConversionModal::StlToSvoConversionModal() {}

StlToSvoConversionModal::~StlToSvoConversionModal() {}

void StlToSvoConversionModal::open(const CADacomb::GUID& assetGuid, const std::string& assetName)
{
    targetAssetGuid = assetGuid;
    targetAssetName = assetName;
    deleteOrigin    = false;
    shouldOpenPopup = true;
}

void StlToSvoConversionModal::render(GatedDatabase& gatedDatabase)
{
    if (shouldOpenPopup)
    {
        ImGui::OpenPopup("Convert to SVO##StlModal");
        shouldOpenPopup = false;
    }

    if (ImGui::BeginPopupModal("Convert to SVO##StlModal", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Converting: %s", targetAssetName.c_str());
        ImGui::Spacing();

        resolutionComboBox.render();

        ImGui::Checkbox("Delete original STL asset", &deleteOrigin);
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

void StlToSvoConversionModal::executeConversion(GatedDatabase& gatedDatabase)
{
    CADacomb::GUID guid      = targetAssetGuid;
    uint32_t       dim       = static_cast<uint32_t>(resolutionComboBox.getResolution());
    bool           delOrigin = deleteOrigin;

    Defer::getInstance().defer(
      [&gatedDatabase, guid, dim, delOrigin]()
      {
          gatedDatabase.executeAsync({
            {         "type", "ConvertStlToSvo" },
            {      "version",                 1 },
            {    "assetGuid",   guid.toString() },
            {    "dimension",               dim },
            { "deleteOrigin",         delOrigin }
          });
      });
}
