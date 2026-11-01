#include "Windows/About/ConceptsInfoWindow.h"

#include "Concept/Concept.h"
#include "Concept/ConceptFactory.h"

#include <algorithm>
#include <imgui.h>

ConceptsInfoWindow::ConceptsInfoWindow() {}

ConceptsInfoWindow::~ConceptsInfoWindow() {}

std::string ConceptsInfoWindow::getType() const
{
    return "ConceptsInfo";
}

std::string ConceptsInfoWindow::getDescription() const
{
    return "Window: ConceptsInfo\n"
           "Title: Concepts\n"
           "Purpose: Interactive reference browser explaining core geometry, volumetric data structures, and file formats.\n\n"
           "Parameters:\n"
           "  - Left Pane (IndexPane): Alphabetically sorted list of registered concepts\n"
           "  - Right Pane (ContentPane): Detailed conceptual documentation, specifications, and pipeline usage\n\n"
           "Dispatched Command: None (read-only documentation window)";
}

void ConceptsInfoWindow::loadData()
{
    entries.clear();

    ConceptFactory factory;
    auto           concepts = factory.getAllConcepts();

    for (const auto& conceptItem : concepts)
    {
        if (conceptItem)
        {
            entries.push_back({ conceptItem->getType(), conceptItem->getDescription() });
        }
    }

    std::sort(entries.begin(), entries.end(), [](const std::pair<std::string, std::string>& a, const std::pair<std::string, std::string>& b) { return a.first < b.first; });

    if (selectedIndex >= static_cast<int>(entries.size()))
    {
        selectedIndex = 0;
    }

    loaded = true;
}

void ConceptsInfoWindow::render()
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
    if (ImGui::Begin("Concepts", &openState))
    {
        ImGui::BeginChild("IndexPane", ImVec2(260.0f, 0.0f), true);
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

TEST_CASE("ConceptsInfoWindow/ConstructionAndType")
{
    ConceptsInfoWindow win;
    REQUIRE(win.getType() == "ConceptsInfo");
    REQUIRE(!win.getDescription().empty());
}
#endif
