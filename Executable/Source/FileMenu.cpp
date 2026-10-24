#include "FileMenu.h"

#include "CADacomb/Triangle/Triangulation.h"
#include "Library/Util/LoadFileDialog.h"
#include "Project/CadAsset.h"
#include "Project/Project.h"
#include "Project/TriangulationAsset.h"
#include "UI/Preferences/PreferencesWindow.h"

#include <filesystem>
#include <imgui.h>

FileMenu::FileMenu() {}

FileMenu::~FileMenu() {}

void FileMenu::render(Project& project, PreferencesWindow& preferencesWindow, bool& quitRequested)
{
    if (ImGui::BeginMenu("File"))
    {
        if (ImGui::BeginMenu("Load"))
        {
            if (ImGui::MenuItem("STL..."))
            {
                openStlFileDialog(project);
            }
            if (ImGui::MenuItem("STEP..."))
            {
                openStepFileDialog(project);
            }
            ImGui::EndMenu();
        }

        ImGui::Separator();
        if (ImGui::MenuItem("Preferences..."))
        {
            preferencesWindow.setOpen(true);
        }

        ImGui::Separator();
        if (ImGui::MenuItem("Quit", "Alt+F4"))
        {
            quitRequested = true;
        }

        ImGui::EndMenu();
    }
}

void FileMenu::openStlFileDialog(Project& project)
{
    Library::LoadFileDialog dialog;
    dialog.addFilter("STL File", { "stl" });
    dialog.execute();

    if (!dialog.isCancled() && !dialog.getResultPath().empty())
    {
        std::filesystem::path filePath(dialog.getResultPath());
        std::string           fileName = filePath.filename().string();
        auto                  tri      = CADacomb::Triangulation::fromSTLFile(dialog.getResultPath());
        auto                  asset    = std::make_shared<TriangulationAsset>(fileName, std::move(tri));
        project.addAsset(asset);
        project.setSelectedIndex(static_cast<int>(project.getAssetCount()) - 1);
    }
}

void FileMenu::openStepFileDialog(Project& project)
{
    Library::LoadFileDialog dialog;
    dialog.addFilter("STEP File", { "step", "stp" });
    dialog.execute();

    if (!dialog.isCancled() && !dialog.getResultPath().empty())
    {
        std::filesystem::path filePath(dialog.getResultPath());
        std::string           fileName = filePath.filename().string();
        auto                  asset    = std::make_shared<CadAsset>(fileName, dialog.getResultPath());
        project.addAsset(asset);
        project.setSelectedIndex(static_cast<int>(project.getAssetCount()) - 1);
    }
}
