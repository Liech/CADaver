#include "Windows/About/CommandsInfoWindow.h"

#include "Execution/Command.h"
#include "Execution/CommandFactory.h"
#include "Execution/GatedDatabase.h"

#include <algorithm>
#include <imgui.h>

CommandsInfoWindow::CommandsInfoWindow() {}

CommandsInfoWindow::~CommandsInfoWindow() {}

std::string CommandsInfoWindow::getType() const
{
    return "CommandsInfo";
}

std::string CommandsInfoWindow::getDescription() const
{
    return "Window: CommandsInfo\n"
           "Title: Commands\n"
           "Purpose: Interactive reference browser displaying all commands registered in GatedDatabase's CommandFactory along with their JSON input/output schemas and stream requirements.\n\n"
           "Parameters:\n"
           "  - Left Pane (IndexPane): Alphabetically sorted list of registered command types\n"
           "  - Right Pane (ContentPane): Full command specification (version, purpose, JSON input syntax, streams, and JSON output syntax)\n\n"
           "Dispatched Command: None (read-only documentation window)";
}

void CommandsInfoWindow::selectCommand(const std::string& commandType)
{
    if (!loaded)
    {
        loadData();
    }

    for (size_t i = 0; i < entries.size(); i++)
    {
        if (entries[i].first == commandType)
        {
            selectedIndex     = static_cast<int>(i);
            scrollToSelection = true;
            break;
        }
    }
}

void CommandsInfoWindow::loadData()
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

void CommandsInfoWindow::render()
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
        handleEscapeClose();
        ImGui::BeginChild("IndexPane", ImVec2(240.0f, 0.0f), true);
        for (size_t i = 0; i < entries.size(); i++)
        {
            bool isSelected = (selectedIndex == static_cast<int>(i));
            if (isSelected && scrollToSelection)
            {
                ImGui::SetScrollHereY(0.5f);
                scrollToSelection = false;
            }
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
            ImGui::PushTextWrapPos(0.0f);
            ImGui::TextUnformatted(entries[selectedIndex].second.c_str());
            ImGui::PopTextWrapPos();
        }
        ImGui::EndChild();
    }
    ImGui::End();
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("CommandsInfoWindow/ConstructionAndType")
{
    CommandsInfoWindow win;
    REQUIRE(win.getType() == "CommandsInfo");
    REQUIRE(!win.getDescription().empty());
    win.selectCommand("NonExistent");
}
#endif
