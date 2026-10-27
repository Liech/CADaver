#include "SvoToStlConversionModal.h"

#include "Commands/Defer.h"
#include "Commands/GatedDatabase.h"

#include <algorithm>
#include <imgui.h>

SvoToStlConversionModal::SvoToStlConversionModal() {}

SvoToStlConversionModal::~SvoToStlConversionModal() {}

void SvoToStlConversionModal::open(int assetIndex, const std::string& assetName, int maxDepth)
{
    targetAssetIndex = assetIndex;
    targetAssetName  = assetName;
    maxTreeDepth     = std::max(1, maxDepth);
    targetDepth      = maxTreeDepth;
    deleteOrigin     = false;
    shouldOpenPopup  = true;
}

void SvoToStlConversionModal::render()
{
    if (shouldOpenPopup)
    {
        ImGui::OpenPopup("Convert SVO to STL##Modal");
        shouldOpenPopup = false;
    }

    if (ImGui::BeginPopupModal("Convert SVO to STL##Modal", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Convert SVO '%s' to STL (Blocky)", targetAssetName.c_str());
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::SliderInt("LOD Depth", &targetDepth, 1, maxTreeDepth);
        ImGui::TextDisabled("Target octree depth level (Max: %d).", maxTreeDepth);

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

void SvoToStlConversionModal::executeConversion()
{
    int index = targetAssetIndex;
    if (index < 0)
    {
        return;
    }

    int  depth        = targetDepth;
    bool shouldDelete = deleteOrigin;
    Defer::getInstance().defer(
      [index, depth, shouldDelete]()
      {
          GatedDatabase::getInstance().execute({
            {         "type", "ConvertSvoToStl" },
            {      "version",                 1 },
            {   "assetIndex",             index },
            {  "targetDepth",             depth },
            { "deleteOrigin",      shouldDelete }
          });
      });
}
