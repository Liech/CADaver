#include "UI/Menu/FileMenu.h"

#include "Commands/GatedDatabase.h"
#include "Library/Util/LoadFileDialog.h"
#include "UI/Preferences/PreferencesWindow.h"

#include <filesystem>
#include <fstream>
#include <imgui.h>

FileMenu::FileMenu() {}

FileMenu::~FileMenu() {}

void FileMenu::render(PreferencesWindow& preferencesWindow, bool& quitRequested)
{
    if (ImGui::BeginMenu("File"))
    {
        if (ImGui::BeginMenu("Load"))
        {
            if (ImGui::MenuItem("STL..."))
            {
                openStlFileDialog();
            }
            if (ImGui::MenuItem("STEP..."))
            {
                openStepFileDialog();
            }
            if (ImGui::MenuItem("SVO..."))
            {
                openSvoFileDialog();
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

void FileMenu::openStlFileDialog()
{
    Library::LoadFileDialog dialog;
    dialog.addFilter("STL File", { "stl" });
    dialog.execute();

    if (!dialog.isCancled() && !dialog.getResultPath().empty())
    {
        std::ifstream fileStream(dialog.getResultPath(), std::ios::binary);
        std::string   fileName = std::filesystem::path(dialog.getResultPath()).filename().string();

        GatedDatabase::getInstance().execute(
          {
            {    "type", "LoadStlAsset" },
            { "version",              1 },
            {    "name",       fileName }
        },
          &fileStream);
    }
}

void FileMenu::openStepFileDialog()
{
    Library::LoadFileDialog dialog;
    dialog.addFilter("STEP File", { "step", "stp" });
    dialog.execute();

    if (!dialog.isCancled() && !dialog.getResultPath().empty())
    {
        std::string fileName = std::filesystem::path(dialog.getResultPath()).filename().string();

        GatedDatabase::getInstance().execute({
          {     "type",        "LoadStepAsset" },
          {  "version",                      1 },
          {     "name",               fileName },
          { "filePath", dialog.getResultPath() }
        });
    }
}

void FileMenu::openSvoFileDialog()
{
    Library::LoadFileDialog dialog;
    dialog.addFilter("SVO File", { "cadacombsvo", "svo" });
    dialog.execute();

    if (!dialog.isCancled() && !dialog.getResultPath().empty())
    {
        std::ifstream fileStream(dialog.getResultPath(), std::ios::binary);
        std::string   fileName = std::filesystem::path(dialog.getResultPath()).filename().string();

        GatedDatabase::getInstance().execute(
          {
            {    "type", "LoadSvoAsset" },
            { "version",              1 },
            {    "name",       fileName }
        },
          &fileStream);
    }
}
