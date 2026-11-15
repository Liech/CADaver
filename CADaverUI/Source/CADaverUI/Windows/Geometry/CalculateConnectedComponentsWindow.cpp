#include "Windows/Geometry/CalculateConnectedComponentsWindow.h"

#include "Execution/Defer.h"
#include "Execution/GatedDatabase.h"

#include <chrono>
#include <imgui.h>
#include <nlohmann/json.hpp>

CalculateConnectedComponentsWindow::CalculateConnectedComponentsWindow()
  : assetComboBox("Triangulation")
{
}

CalculateConnectedComponentsWindow::~CalculateConnectedComponentsWindow() {}

std::string CalculateConnectedComponentsWindow::getType() const
{
    return "CalculateConnectedComponents";
}

std::string CalculateConnectedComponentsWindow::getDescription() const
{
    return "Window: CalculateConnectedComponents\n"
           "Title: Calculate Connected Components\n"
           "Purpose: Non-modal dialog for calculating connected components on a Triangulation asset and optionally splitting them into separate assets.\n\n"
           "Parameters:\n"
           "  - Target Asset: Triangulation asset to analyze\n"
           "  - Split into separate assets: Run SplitClustersToAssets after clustering finishes\n\n"
           "Dispatched Commands: CalculateConnectedComponents (async), SplitClustersToAssets (async, optional)";
}

void CalculateConnectedComponentsWindow::render()
{
    if (pendingFuture.valid() && pendingFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
    {
        Polyglot::Node result = pendingFuture.get();
        nlohmann::json json   = result.toJson();
        if (gatedDatabase && defer && json.value("type", "") == "CalculateConnectedComponents" && json.contains("componentGuid"))
        {
            GatedDatabase* db            = gatedDatabase;
            CADacomb::GUID targetGuid    = pendingAssetGuid;
            std::string    componentGuid = json.at("componentGuid").get<std::string>();
            defer->defer(
              [db, targetGuid, componentGuid]()
              {
                  Polyglot::Node splitCmd = {
                      {          "type", "SplitClustersToAssets" },
                      {       "version",                       1 },
                      {     "assetGuid",   targetGuid.toString() },
                      { "componentGuid",           componentGuid }
                  };
                  db->executeAsync(splitCmd);
              });
        }
        pendingAssetGuid = CADacomb::GUID();
    }

    if (!openState || !gatedDatabase)
    {
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(420.0f, 175.0f), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Calculate Connected Components", &openState))
    {
        handleEscapeClose();

        assetComboBox.render(*gatedDatabase);

        ImGui::Checkbox("Split into separate assets", &splitIntoAssets);

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        bool canCalculate = !assetComboBox.getSelectedGuid().toString().empty();
        ImGui::BeginDisabled(!canCalculate);
        if (ImGui::Button("Calculate", ImVec2(120.0f, 0.0f)))
        {
            executeCalculate();
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

void CalculateConnectedComponentsWindow::setTargetAsset(const CADacomb::GUID& assetGuid)
{
    assetComboBox.setSelectedGuid(assetGuid);
}

void CalculateConnectedComponentsWindow::executeCalculate()
{
    CADacomb::GUID selectedGuid = assetComboBox.getSelectedGuid();
    if (!gatedDatabase || !defer || selectedGuid.toString().empty())
    {
        return;
    }

    GatedDatabase* db      = gatedDatabase;
    bool           doSplit = splitIntoAssets;
    defer->defer(
      [this, db, selectedGuid, doSplit]()
      {
          Polyglot::Node command = {
              {      "type", "CalculateConnectedComponents" },
              {   "version",                              1 },
              { "assetGuid",        selectedGuid.toString() }
          };
          std::future<Polyglot::Node> future = db->executeAsync(command);
          if (doSplit)
          {
              pendingAssetGuid = selectedGuid;
              pendingFuture    = std::move(future);
          }
      });
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("CalculateConnectedComponentsWindow/TypeAndOpenState")
{
    CalculateConnectedComponentsWindow window;
    CHECK(window.getType() == "CalculateConnectedComponents");
    CHECK(!window.isOpen());
    window.open();
    CHECK(window.isOpen());
    window.close();
    CHECK(!window.isOpen());
}
#endif
