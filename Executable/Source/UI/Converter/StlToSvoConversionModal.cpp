#include "StlToSvoConversionModal.h"

#include "Commands/Defer.h"
#include "Commands/GatedDatabase.h"

#include <imgui.h>

StlToSvoConversionModal::StlToSvoConversionModal() {}

StlToSvoConversionModal::~StlToSvoConversionModal() {}

void StlToSvoConversionModal::open(int assetIndex, const std::string& assetName)
{
    targetAssetIndex = assetIndex;
    targetAssetName  = assetName;
    deleteOrigin     = false;
    resolutionComboBox.setResolution(64);
    shouldOpenPopup = true;
}

void StlToSvoConversionModal::render()
{
    if (shouldOpenPopup)
    {
        ImGui::OpenPopup("Convert STL to SVO##Modal");
        shouldOpenPopup = false;
    }

    if (ImGui::BeginPopupModal("Convert STL to SVO##Modal", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Convert STL '%s' to SVO", targetAssetName.c_str());
        ImGui::Spacing();
        ImGui::Separator();
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

void StlToSvoConversionModal::executeConversion()
{
    int index = targetAssetIndex;
    if (index < 0)
    {
        return;
    }

    uint32_t dim          = resolutionComboBox.getResolution();
    bool     shouldDelete = deleteOrigin;
    Defer::getInstance().defer(
      [index, dim, shouldDelete]()
      {
          GatedDatabase::getInstance().execute(
            {
              {         "type", "ConvertStlToSvo" },
              {      "version",                 1 },
              {   "assetIndex",             index },
              {    "dimension",               dim },
              { "deleteOrigin",      shouldDelete }
          },
            true);
      });
}
