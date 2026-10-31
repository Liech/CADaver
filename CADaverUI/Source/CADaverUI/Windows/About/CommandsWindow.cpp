#include "Windows/About/CommandsWindow.h"

#include "Execution/Command.h"
#include "Execution/CommandFactory.h"
#include "Execution/GatedDatabase.h"

#include <algorithm>
#include <imgui.h>

CommandsWindow::CommandsWindow() {}

CommandsWindow::~CommandsWindow() {}

std::string CommandsWindow::getType() const
{
    return "CommandsWindow";
}

std::string CommandsWindow::getDescription() const
{
    return "Window: CommandsWindow\n"
           "Title: Commands\n"
           "Purpose: Interactive reference browser displaying all commands registered in GatedDatabase's CommandFactory along with their JSON input/output schemas and stream requirements.\n\n"
           "Parameters:\n"
           "  - Left Pane (IndexPane): Alphabetically sorted list of registered command types\n"
           "  - Right Pane (ContentPane): Full command specification (version, purpose, JSON input syntax, streams, and JSON output syntax)\n\n"
           "Dispatched Command: None (read-only documentation window)";
}

bool CommandsWindow::isOpen() const
{
    return openState;
}

void CommandsWindow::open()
{
    loaded    = false;
    openState = true;
}

void CommandsWindow::close()
{
    openState = false;
}

void CommandsWindow::loadData()
{
    entries.clear();

    if (!gatedDatabase)
    {
        return;
    }

    auto commands = gatedDatabase->getCommandFactory().getAllCommands();

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

void CommandsWindow::render()
{
    if (!openState || !gatedDatabase)
    {
        return;
    }

    if (!loaded)
    {
        loadData();
    }

    ImGui::SetNextWindowSize(ImVec2(800.0f, 520.0f), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Commands", &openState))
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
