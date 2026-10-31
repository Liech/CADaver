#include "Windows/About/AboutWindow.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <imgui.h>
#include <sstream>

AboutWindow::AboutWindow() {}

AboutWindow::~AboutWindow() {}

std::string AboutWindow::getType() const
{
    return "AboutWindow";
}

std::string AboutWindow::getDescription() const
{
    return "Window: AboutWindow\n"
           "Title: About CADaver\n"
           "Purpose: Displays application overview information from about.txt along with a two-pane browser of all third-party software licenses in the Licenses/ directory.\n\n"
           "Parameters:\n"
           "  - Left Pane (IndexPane): Selectable list of 'CADaver' and all discovered third-party license files\n"
           "  - Right Pane (ContentPane): Scrollable full text of the selected about/license document\n\n"
           "Dispatched Command: None (read-only informational window)";
}

std::string AboutWindow::readFileContent(const std::string& filePath) const
{
    std::ifstream stream(filePath);
    if (!stream.is_open())
    {
        return "";
    }

    std::stringstream buffer;
    buffer << stream.rdbuf();
    return buffer.str();
}

void AboutWindow::loadData()
{
    entries.clear();

    std::string aboutText = readFileContent("about.txt");
    entries.push_back({ "CADaver", aboutText });

    std::string licensesFolder = "Licenses";
    if (std::filesystem::exists(licensesFolder) && std::filesystem::is_directory(licensesFolder))
    {
        std::vector<std::pair<std::string, std::string>> licenseEntries;

        for (const auto& dirEntry : std::filesystem::directory_iterator(licensesFolder))
        {
            if (dirEntry.is_regular_file())
            {
                std::string title   = dirEntry.path().stem().string();
                std::string content = readFileContent(dirEntry.path().string());
                licenseEntries.push_back({ title, content });
            }
        }

        std::sort(licenseEntries.begin(), licenseEntries.end(), [](const std::pair<std::string, std::string>& a, const std::pair<std::string, std::string>& b) { return a.first < b.first; });

        for (const auto& entry : licenseEntries)
        {
            entries.push_back(entry);
        }
    }

    if (selectedIndex >= static_cast<int>(entries.size()))
    {
        selectedIndex = 0;
    }

    loaded = true;
}

void AboutWindow::render()
{
    if (!openState)
    {
        return;
    }

    if (!loaded)
    {
        loadData();
    }

    ImGui::SetNextWindowSize(ImVec2(800.0f, 520.0f), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("About CADaver", &openState))
    {
        ImGui::BeginChild("IndexPane", ImVec2(240.0f, 0.0f), true);
        for (size_t i = 0; i < entries.size(); i++)
        {
            bool isSelected = (selectedIndex == static_cast<int>(i));
            if (ImGui::Selectable(entries[i].first.c_str(), isSelected))
            {
                selectedIndex = static_cast<int>(i);
            }
        }
        ImGui::EndChild();

        ImGui::SameLine();

        ImGui::BeginChild("ContentPane", ImVec2(0.0f, 0.0f), true, ImGuiWindowFlags_HorizontalScrollbar);
        if (selectedIndex >= 0 && selectedIndex < static_cast<int>(entries.size()))
        {
            ImGui::TextUnformatted(entries[selectedIndex].second.c_str());
        }
        ImGui::EndChild();
    }
    ImGui::End();
}
