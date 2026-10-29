#include "UI/Converter/SvoToStlConversionModal.h"

#include "Commands/Defer.h"
#include "Commands/GatedDatabase.h"

#include <imgui.h>

SvoToStlConversionModal::SvoToStlConversionModal() {}

SvoToStlConversionModal::~SvoToStlConversionModal() {}

void SvoToStlConversionModal::open(const CADacomb::GUID& assetGuid, const std::string& assetName, int maxDepth)
{
    targetAssetGuid = assetGuid;
    targetAssetName = assetName;
    targetDepth     = maxDepth;
    maxTreeDepth    = maxDepth;
    deleteOrigin    = false;
    shouldOpenPopup = true;
}

void SvoToStlConversionModal::render()
{
    if (shouldOpenPopup)
    {
        ImGui::OpenPopup("Convert to STL##SvoModal");
        shouldOpenPopup = false;
    }

    if (ImGui::BeginPopupModal("Convert to STL##SvoModal", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Converting: %s", targetAssetName.c_str());
        ImGui::Spacing();

        ImGui::SliderInt("Target LOD Depth", &targetDepth, 0, maxTreeDepth);

        ImGui::Checkbox("Delete original SVO asset", &deleteOrigin);
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

void SvoToStlConversionModal::executeConversion()
{
    CADacomb::GUID guid      = targetAssetGuid;
    int            depth     = targetDepth;
    bool           delOrigin = deleteOrigin;

    Defer::getInstance().defer(
      [guid, depth, delOrigin]()
      {
          GatedDatabase::getInstance().execute(
            {
              {         "type", "ConvertSvoToStl" },
              {      "version",                 1 },
              {    "assetGuid",   guid.toString() },
              {  "targetDepth",             depth },
              { "deleteOrigin",         delOrigin }
          },
            true);
      });
}
