#include "Windows/Conversion/ConvertStlToCadWindow.h"

#include "Execution/Defer.h"
#include "Execution/GatedDatabase.h"

#include <imgui.h>

ConvertStlToCadWindow::ConvertStlToCadWindow() : assetComboBox("Triangulation")
{
}

ConvertStlToCadWindow::~ConvertStlToCadWindow()
{
}

std::string ConvertStlToCadWindow::getType() const
{
    return "ConvertStlToCad";
}

std::string ConvertStlToCadWindow::getDescription() const
{
    return "Window: ConvertStlToCad\n"
           "Title: Convert STL to CAD (Naive)\n"
           "Purpose: Non-modal dialog for converting a Triangulation asset naively (one planar face per triangle) into a CadAsset.\n\n"
           "Parameters:\n"
           "  - Target Asset: Triangulation asset\n"
           "  - Stitch Tolerance: Edge stitching tolerance\n"
           "  - Try Make Solid: Attempt to stitch faces into a closed solid\n\n"
           "Dispatched Command: ConvertStlToCad (async)";
}

void ConvertStlToCadWindow::render()
{
    if (!openState || !gatedDatabase)
    {
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(440.0f, 190.0f), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Convert STL to CAD (Naive)", &openState))
    {
        handleEscapeClose();

        assetComboBox.render(*gatedDatabase);

        ImGui::InputFloat("Stitch Tolerance", &stitchTolerance, 0.0001f, 0.01f, "%.6f");
        ImGui::Checkbox("Try Make Solid", &tryMakeSolid);

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        bool canConvert = !assetComboBox.getSelectedGuid().toString().empty();
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

void ConvertStlToCadWindow::setTargetAsset(const CADacomb::GUID& assetGuid)
{
    assetComboBox.setSelectedGuid(assetGuid);
}

void ConvertStlToCadWindow::executeConvert()
{
    CADacomb::GUID selectedGuid = assetComboBox.getSelectedGuid();
    if (!defer || !gatedDatabase || selectedGuid.toString().empty())
    {
        return;
    }

    GatedDatabase* db        = gatedDatabase;
    double         stitchTol = static_cast<double>(stitchTolerance);
    bool           makeSolid = tryMakeSolid;

    defer->defer(
      [db, selectedGuid, stitchTol, makeSolid]()
      {
          Polyglot::Node command = {
              {            "type",       "ConvertStlToCad" },
              {         "version",                       1 },
              {       "assetGuid", selectedGuid.toString() },
              { "stitchTolerance",               stitchTol },
              {    "tryMakeSolid",               makeSolid }
          };
          db->executeAsync(command);
      });
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("ConvertStlToCadWindow/TypeAndOpenState")
{
    ConvertStlToCadWindow window;
    CHECK(window.getType() == "ConvertStlToCad");
    CHECK(!window.isOpen());
    window.open();
    CHECK(window.isOpen());
    window.close();
    CHECK(!window.isOpen());
}
#endif
