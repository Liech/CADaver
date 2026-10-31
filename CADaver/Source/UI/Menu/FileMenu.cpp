#include "UI/Menu/FileMenu.h"

#include "Execution/Defer.h"
#include "Execution/GatedDatabase.h"
#include "UI/Dialog/LoadFileDialog.h"
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
        std::string path     = dialog.getResultPath();
        std::string fileName = std::filesystem::path(path).filename().string();

        Defer::getInstance().defer(
          [path, fileName]()
          {
              auto fileStream = std::make_shared<std::ifstream>(path, std::ios::binary);
              GatedDatabase::getInstance().executeAsync(
                {
                  {    "type", "LoadStlAsset" },
                  { "version",              1 },
                  {    "name",       fileName }
              },
                true,
                fileStream);
          });
    }
}

void FileMenu::openStepFileDialog()
{
    Library::LoadFileDialog dialog;
    dialog.addFilter("STEP File", { "step", "stp" });
    dialog.execute();

    if (!dialog.isCancled() && !dialog.getResultPath().empty())
    {
        std::string path     = dialog.getResultPath();
        std::string fileName = std::filesystem::path(path).filename().string();

        Defer::getInstance().defer(
          [path, fileName]()
          {
              GatedDatabase::getInstance().executeAsync(
                {
                  {     "type", "LoadStepAsset" },
                  {  "version",               1 },
                  {     "name",        fileName },
                  { "filePath",            path }
              },
                true);
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
        std::string path     = dialog.getResultPath();
        std::string fileName = std::filesystem::path(path).filename().string();

        Defer::getInstance().defer(
          [path, fileName]()
          {
              auto fileStream = std::make_shared<std::ifstream>(path, std::ios::binary);
              GatedDatabase::getInstance().executeAsync(
                {
                  {    "type", "LoadSvoAsset" },
                  { "version",              1 },
                  {    "name",       fileName }
              },
                true,
                fileStream);
          });
    }
}
