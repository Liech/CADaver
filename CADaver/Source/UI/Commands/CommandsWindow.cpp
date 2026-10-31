#include "CommandsWindow.h"

#include "Execution/Command.h"
#include "Execution/CommandFactory.h"
#include "Execution/GatedDatabase.h"

#include <algorithm>
#include <imgui.h>

CommandsWindow::CommandsWindow() {}

CommandsWindow::~CommandsWindow() {}

bool CommandsWindow::isOpen() const
{
    return open;
}

void CommandsWindow::setOpen(bool openState)
{
    if (openState)
    {
        loaded = false;
    }
    open = openState;
}

void CommandsWindow::loadData(const GatedDatabase& gatedDatabase)
{
    entries.clear();

    auto commands = gatedDatabase.getCommandFactory().getAllCommands();

    for (const auto& command : commands)
    {
        if (command)
        {
            entries.push_back({ command->getType(), command->getDescription() });
        }
    }

    std::sort(entries.begin(), entries.end(), [](const std::pair<std::string, std::string>& a, const std::pair<std::string, std::string>& b) { return a.first < b.first; });

    if (selectedIndex >= static_cast<int>(entries.size()))
    {
        selectedIndex = 0;
    }

    loaded = true;
}

void CommandsWindow::render(const GatedDatabase& gatedDatabase)
{
    if (!open)
    {
        return;
    }

    if (!loaded)
    {
        loadData(gatedDatabase);
    }

    ImGui::SetNextWindowSize(ImVec2(800.0f, 520.0f), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Commands", &open))
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
