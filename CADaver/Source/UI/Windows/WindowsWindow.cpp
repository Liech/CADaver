#include "UI/Windows/WindowsWindow.h"

#include "Windows/Window.h"
#include "Windows/WindowFactory.h"

#include <algorithm>
#include <imgui.h>

WindowsWindow::WindowsWindow() {}

WindowsWindow::~WindowsWindow() {}

bool WindowsWindow::isOpen() const
{
    return open;
}

void WindowsWindow::setOpen(bool openState)
{
    if (openState)
    {
        loaded = false;
    }
    open = openState;
}

void WindowsWindow::loadData()
{
    entries.clear();

    WindowFactory factory;
    auto          windows = factory.getAllWindows();

    for (const auto& win : windows)
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

void WindowsWindow::render()
{
    if (!open)
    {
        return;
    }

    if (!loaded)
    {
        loadData();
    }

    ImGui::SetNextWindowSize(ImVec2(800.0f, 520.0f), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Windows", &open))
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
