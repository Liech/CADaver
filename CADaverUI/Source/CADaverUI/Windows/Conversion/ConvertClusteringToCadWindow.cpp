#include "Windows/Conversion/ConvertClusteringToCadWindow.h"

#include "Execution/Defer.h"
#include "Execution/GatedDatabase.h"

#include <imgui.h>

ConvertClusteringToCadWindow::ConvertClusteringToCadWindow() : assetComboBox("Triangulation")
{
}

ConvertClusteringToCadWindow::~ConvertClusteringToCadWindow()
{
}

std::string ConvertClusteringToCadWindow::getType() const
{
    return "ConvertClusteringToCad";
}

std::string ConvertClusteringToCadWindow::getDescription() const
{
    return "Window: ConvertClusteringToCad\n"
           "Title: Convert Clustering to CAD\n"
           "Purpose: Non-modal dialog for converting a clustered Triangulation asset into a CadAsset.\n\n"
           "Parameters:\n"
           "  - Target Asset: Clustered Triangulation asset\n"
           "  - Planarity Tolerance: Maximum vertex-to-plane deviation\n"
           "  - Boundary Simplify Tolerance: Chord tolerance for boundary chain simplification\n"
           "  - Stitch Tolerance: Edge stitching tolerance\n"
           "  - Try Make Solid: Attempt to stitch faces into a closed solid\n\n"
           "Dispatched Command: ConvertClusteringToCad (async)";
}

void ConvertClusteringToCadWindow::render()
{
    if (!openState || !gatedDatabase)
    {
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(460.0f, 250.0f), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Convert Clustering to CAD", &openState))
    {
        handleEscapeClose();

        assetComboBox.render(*gatedDatabase);
        renderComponentComboBox();

        ImGui::InputFloat("Planarity Tolerance", &planarityTolerance, 0.0001f, 0.01f, "%.6f");
        ImGui::InputFloat("Boundary Simplify Tolerance", &boundarySimplifyTolerance, 0.0001f, 0.01f, "%.6f");
        ImGui::InputFloat("Stitch Tolerance", &stitchTolerance, 0.0001f, 0.01f, "%.6f");
        ImGui::Checkbox("Try Make Solid", &tryMakeSolid);

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        bool canConvert = !assetComboBox.getSelectedGuid().toString().empty() && !selectedComponentGuid.toString().empty();
        ImGui::BeginDisabled(!canConvert);
        if (ImGui::Button("Convert", ImVec2(120.0f, 0.0f)))
        {
            executeConvert();
        }
        ImGui::EndDisabled();

        ImGui::SameLine();
        if (ImGui::Button("Close", ImVec2(120.0f, 0.0f)))
        {
            close();
        }
    }
    ImGui::End();
}

void ConvertClusteringToCadWindow::setTargetAsset(const CADacomb::GUID& assetGuid)
{
    assetComboBox.setSelectedGuid(assetGuid);
    selectedComponentGuid = CADacomb::GUID("");
}

void ConvertClusteringToCadWindow::renderComponentComboBox()
{
    CADacomb::GUID selectedAssetGuid = assetComboBox.getSelectedGuid();
    if (!gatedDatabase || selectedAssetGuid.toString().empty())
    {
        selectedComponentGuid = CADacomb::GUID("");
        ImGui::BeginDisabled();
        if (ImGui::BeginCombo("Clustering", "No asset selected"))
        {
            ImGui::EndCombo();
        }
        ImGui::EndDisabled();
        return;
    }

    Polyglot::Node response   = gatedDatabase->execute({
      {    "type", "GetAssets" },
      { "version",           1 }
    });
    nlohmann::json assetsJson = response.toJson().value("assets", nlohmann::json::array());

    std::vector<std::pair<CADacomb::GUID, std::string>> matchingComponents;
    for (const auto& assetJson : assetsJson)
    {
        if (assetJson.value("assetGuid", "") != selectedAssetGuid.toString())
        {
            continue;
        }
        const nlohmann::json& components = assetJson.value("components", nlohmann::json::array());
        for (const auto& comp : components)
        {
            if (comp.value("type", "") == "FaceValues")
            {
                std::string compGuid = comp.value("componentGuid", "");
                std::string compName = comp.value("name", "");
                if (compName.empty())
                {
                    compName = compGuid;
                }
                matchingComponents.emplace_back(CADacomb::GUID(compGuid), compName);
            }
        }
        break;
    }

    if (matchingComponents.empty())
    {
        selectedComponentGuid = CADacomb::GUID("");
        ImGui::BeginDisabled();
        if (ImGui::BeginCombo("Clustering", "No FaceValues components"))
        {
            ImGui::EndCombo();
        }
        ImGui::EndDisabled();
        return;
    }

    int currentIdx = -1;
    for (size_t i = 0; i < matchingComponents.size(); i++)
    {
        if (matchingComponents[i].first == selectedComponentGuid)
        {
            currentIdx = static_cast<int>(i);
            break;
        }
    }

    if (currentIdx < 0)
    {
        currentIdx            = 0;
        selectedComponentGuid = matchingComponents.front().first;
    }

    std::string previewLabel = matchingComponents[static_cast<size_t>(currentIdx)].second;
    if (ImGui::BeginCombo("Clustering", previewLabel.c_str()))
    {
        for (size_t i = 0; i < matchingComponents.size(); i++)
        {
            bool isSelected = (static_cast<int>(i) == currentIdx);
            if (ImGui::Selectable(matchingComponents[i].second.c_str(), isSelected))
            {
                selectedComponentGuid = matchingComponents[i].first;
            }
            if (isSelected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
}

void ConvertClusteringToCadWindow::executeConvert()
{
    CADacomb::GUID selectedGuid = assetComboBox.getSelectedGuid();
    if (!gatedDatabase || !defer || selectedGuid.toString().empty() || selectedComponentGuid.toString().empty())
    {
        return;
    }

    GatedDatabase* db            = gatedDatabase;
    CADacomb::GUID compGuid      = selectedComponentGuid;
    double         planarityTol  = static_cast<double>(planarityTolerance);
    double         simplifyTol   = static_cast<double>(boundarySimplifyTolerance);
    double         stitchTol     = static_cast<double>(stitchTolerance);
    bool           makeSolid     = tryMakeSolid;

    defer->defer(
      [db, selectedGuid, compGuid, planarityTol, simplifyTol, stitchTol, makeSolid]()
      {
          Polyglot::Node command = {
              {                      "type", "ConvertClusteringToCad" },
              {                   "version",                        1 },
              {                 "assetGuid",  selectedGuid.toString() },
              {             "componentGuid",      compGuid.toString() },
              {        "planarityTolerance",             planarityTol },
              { "boundarySimplifyTolerance",              simplifyTol },
              {           "stitchTolerance",                stitchTol },
              {              "tryMakeSolid",                makeSolid }
          };
          db->executeAsync(command);
      });
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("ConvertClusteringToCadWindow/TypeAndOpenState")
{
    ConvertClusteringToCadWindow window;
    CHECK(window.getType() == "ConvertClusteringToCad");
    CHECK(!window.isOpen());
    window.open();
    CHECK(window.isOpen());
    window.close();
    CHECK(!window.isOpen());
}
#endif
