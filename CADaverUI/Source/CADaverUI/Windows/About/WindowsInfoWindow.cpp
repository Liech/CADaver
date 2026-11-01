#include "Windows/About/WindowsInfoWindow.h"

#include "Windows/Window.h"
#include "Windows/WindowFactory.h"
#include "Windows/Windows.h"

#include <algorithm>
#include <imgui.h>

WindowsInfoWindow::WindowsInfoWindow() {}

WindowsInfoWindow::~WindowsInfoWindow() {}

std::string WindowsInfoWindow::getType() const
{
    return "WindowsInfo";
}

std::string WindowsInfoWindow::getDescription() const
{
    return "Window: WindowsInfo\n"
           "Title: Windows\n"
           "Purpose: Interactive reference browser displaying all UI windows registered in WindowFactory along with their purpose, parameters, and dispatched commands.\n\n"
           "Parameters:\n"
           "  - Left Pane (IndexPane): Alphabetically sorted list of registered Window types\n"
           "  - Right Pane (ContentPane): Open button and full window specification (title, purpose, UI controls/parameters, and dispatched commands)\n\n"
           "Dispatched Command: None (read-only documentation window)";
}

void WindowsInfoWindow::loadData()
{
    entries.clear();

    WindowFactory factory;
    auto          windowList = factory.getAllWindows();

    for (const auto& win : windowList)
    {
        if (win)
        {
            entries.push_back({ win->getType(), win->getDescription() });
        }
    }

    std::sort(entries.begin(), entries.end(), [](const std::pair<std::string, std::string>& a, const std::pair<std::string, std::string>& b) { return a.first < b.first; });

    if (selectedIndex >= static_cast<int>(entries.size()))
    {
        selectedIndex = 0;
    }

    loaded = true;
}

void WindowsInfoWindow::render()
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
    if (ImGui::Begin("Windows", &openState))
    {
        handleEscapeClose();
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

        ImGui::BeginChild("ContentPane", ImVec2(0.0f, 0.0f), true);
        if (selectedIndex >= 0 && selectedIndex < static_cast<int>(entries.size()))
        {
            if (windows && ImGui::Button("Open", ImVec2(120.0f, 0.0f)))
            {
                windows->open(entries[selectedIndex].first);
            }
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::PushTextWrapPos(0.0f);
            ImGui::TextUnformatted(entries[selectedIndex].second.c_str());
            ImGui::PopTextWrapPos();
        }
        ImGui::EndChild();
    }
    ImGui::End();
}
