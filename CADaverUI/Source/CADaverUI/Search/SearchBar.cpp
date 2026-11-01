#include "Search/SearchBar.h"

#include "Execution/Defer.h"
#include "Execution/GatedDatabase.h"
#include "Search/AssetSearcher.h"
#include "Search/CommandSearcher.h"
#include "Search/ConceptSearcher.h"
#include "Search/UIElementSearcher.h"
#include "Search/WindowSearcher.h"
#include "Windows/Windows.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <imgui.h>
#include <sstream>

SearchBar::SearchBar() {}

SearchBar::~SearchBar() {}

void SearchBar::init(GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windowsRef, bool& quitRequested)
{
    if (initialized)
    {
        return;
    }

    windows = &windowsRef;
    uiElementFactory.registerDefaults(gatedDatabase, undoStack, windowsRef, quitRequested);

    searchService.addSearcher(std::make_unique<UIElementSearcher>(uiElementFactory));
    searchService.addSearcher(std::make_unique<AssetSearcher>(gatedDatabase.get()));
    searchService.addSearcher(std::make_unique<WindowSearcher>(windowsRef));
    searchService.addSearcher(std::make_unique<ConceptSearcher>());
    searchService.addSearcher(std::make_unique<CommandSearcher>(gatedDatabase.getCommandFactory()));

    initialized = true;
}

void SearchBar::renderSearchIcon() const
{
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2      p        = ImGui::GetCursorScreenPos();
    float       frameH   = ImGui::GetFrameHeight();
    float       radius   = frameH * 0.22f;
    ImVec2      center   = ImVec2(p.x + frameH * 0.35f, p.y + frameH * 0.5f - 1.0f);
    ImU32       iconCol  = ImGui::GetColorU32(ImGuiCol_TextDisabled);

    drawList->AddCircle(center, radius, iconCol, 16, 1.5f);
    ImVec2 handleStart = ImVec2(center.x + radius * 0.707f, center.y + radius * 0.707f);
    ImVec2 handleEnd   = ImVec2(handleStart.x + radius * 0.85f, handleStart.y + radius * 0.85f);
    drawList->AddLine(handleStart, handleEnd, iconCol, 2.0f);

    ImGui::Dummy(ImVec2(frameH * 0.7f, frameH));
    ImGui::SameLine();
}

void SearchBar::render(Defer& defer)
{
    float barWidth = 260.0f;

    renderSearchIcon();

    std::array<char, 256> buffer{};
    std::copy_n(query.c_str(), std::min(query.size(), buffer.size() - 1), buffer.data());

    ImGui::SetNextItemWidth(barWidth);
    bool textChanged = ImGui::InputTextWithHint("##GlobalSearchInput", "Search features, assets, notes...", buffer.data(), buffer.size());
    if (textChanged)
    {
        query = buffer.data();
    }

    bool   isInputActive  = ImGui::IsItemActive();
    bool   isInputFocused = ImGui::IsItemFocused();
    ImVec2 inputMin       = ImGui::GetItemRectMin();
    ImVec2 inputMax       = ImGui::GetItemRectMax();

    if (query != lastQuery)
    {
        lastQuery = query;
        if (!query.empty())
        {
            results       = searchService.search(query);
            selectedIndex = -1;
            isOpen        = true;
        }
        else
        {
            results.clear();
            isOpen = false;
        }
    }

    if ((isInputActive || isInputFocused) && !query.empty())
    {
        isOpen = true;
    }

    if (isOpen)
    {
        float  popupWidth    = 540.0f;
        ImVec2 popupPos      = ImVec2(inputMin.x, inputMax.y + 4.0f);
        float  displayH      = ImGui::GetIO().DisplaySize.y;
        float  maxAvailableH = std::max(160.0f, displayH - popupPos.y - 16.0f);

        if (popupPos.x + popupWidth > ImGui::GetIO().DisplaySize.x)
        {
            popupPos.x = std::max(10.0f, ImGui::GetIO().DisplaySize.x - popupWidth - 10.0f);
        }

        ImGui::SetNextWindowPos(popupPos, ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(popupWidth, 0.0f), ImGuiCond_Always);

        ImGuiWindowFlags popupFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing;

        if (ImGui::Begin("##SearchDropdownPopup", nullptr, popupFlags))
        {
            renderDropdown(defer, maxAvailableH);
        }
        ImGui::End();
    }
}

void SearchBar::renderDropdown(Defer& defer, float availableHeight)
{
    if (ImGui::IsKeyPressed(ImGuiKey_Escape))
    {
        isOpen = false;
        return;
    }

    if (!results.empty())
    {
        if (ImGui::IsKeyPressed(ImGuiKey_DownArrow))
        {
            selectedIndex = std::min(static_cast<int>(results.size()) - 1, selectedIndex + 1);
        }
        if (ImGui::IsKeyPressed(ImGuiKey_UpArrow))
        {
            selectedIndex = std::max(0, selectedIndex - 1);
        }
        if (ImGui::IsKeyPressed(ImGuiKey_Enter) && selectedIndex >= 0 && selectedIndex < static_cast<int>(results.size()))
        {
            executeSelection(static_cast<size_t>(selectedIndex), defer);
            return;
        }
    }

    if (results.empty())
    {
        ImGui::TextDisabled("No matching features, commands, or assets.");
        return;
    }

    auto  queryTokens  = tokenizeQuery(query);
    float neededHeight = static_cast<float>(results.size()) * 95.0f + 30.0f;
    float childHeight  = std::min(availableHeight - 16.0f, neededHeight);

    ImGui::BeginChild("##ResultsScrollArea", ImVec2(0.0f, childHeight), false);
    for (size_t i = 0; i < results.size(); i++)
    {
        bool isSelected = (selectedIndex == static_cast<int>(i));
        renderResultCard(i, results[i], isSelected, queryTokens, defer);
        if (!isOpen)
        {
            break;
        }
    }
    ImGui::EndChild();
}

void SearchBar::renderResultCard(size_t index, const SearchResult& item, bool isSelected, const std::vector<std::string>& queryTokens, Defer& defer)
{
    ImGui::PushID(static_cast<int>(index));

    ImVec4 badgeColor(0.7f, 0.7f, 0.7f, 1.0f);
    if (item.getCategory() == "Window")
    {
        badgeColor = ImVec4(0.3f, 0.85f, 0.4f, 1.0f);
    }
    else if (item.getCategory() == "Command")
    {
        badgeColor = ImVec4(0.4f, 0.7f, 1.0f, 1.0f);
    }
    else if (item.getCategory() == "Asset")
    {
        badgeColor = ImVec4(1.0f, 0.8f, 0.3f, 1.0f);
    }
    else if (item.getCategory() == "Note")
    {
        badgeColor = ImVec4(0.9f, 0.5f, 0.9f, 1.0f);
    }
    else if (item.getCategory() == "Concept")
    {
        badgeColor = ImVec4(0.95f, 0.42f, 0.42f, 1.0f);
    }
    else if (item.getCategory() == "UI" || item.getCategory() == "Menu")
    {
        badgeColor = ImVec4(0.25f, 0.85f, 0.85f, 1.0f);
    }

    std::string badge = "[" + item.getCategory() + "] ";
    ImGui::TextColored(badgeColor, "%s", badge.c_str());
    ImGui::SameLine();

    if (ImGui::Selectable(item.getTitle().c_str(), isSelected))
    {
        executeSelection(index, defer);
        ImGui::PopID();
        return;
    }

    auto lines = extractContextLines(item.getDescription(), queryTokens, 4);
    if (!lines.empty())
    {
        ImGui::Indent(16.0f);
        ImGui::SetWindowFontScale(0.88f);
        for (const auto& line : lines)
        {
            renderHighlightedLine(line, queryTokens);
        }
        ImGui::SetWindowFontScale(1.0f);
        ImGui::Unindent(16.0f);
    }

    ImGui::Spacing();
    ImGui::Separator();

    ImGui::PopID();
}

void SearchBar::renderHighlightedLine(const std::string& line, const std::vector<std::string>& queryTokens) const
{
    auto ranges = findMatchRanges(line, queryTokens);
    if (ranges.empty())
    {
        ImGui::TextDisabled("%s", line.c_str());
        return;
    }

    size_t currentIdx = 0;
    ImVec4 matchColor(1.0f, 0.88f, 0.25f, 1.0f);

    for (const auto& range : ranges)
    {
        if (range.first > currentIdx)
        {
            std::string normalChunk = line.substr(currentIdx, range.first - currentIdx);
            ImGui::TextDisabled("%s", normalChunk.c_str());
            ImGui::SameLine(0.0f, 0.0f);
        }

        if (range.second > range.first)
        {
            std::string matchChunk = line.substr(range.first, range.second - range.first);
            ImGui::TextColored(matchColor, "%s", matchChunk.c_str());
            ImGui::SameLine(0.0f, 0.0f);
        }

        currentIdx = range.second;
    }

    if (currentIdx < line.size())
    {
        std::string trailingChunk = line.substr(currentIdx);
        ImGui::TextDisabled("%s", trailingChunk.c_str());
    }
    else
    {
        ImGui::NewLine();
    }
}

std::vector<std::pair<size_t, size_t>> SearchBar::findMatchRanges(const std::string& text, const std::vector<std::string>& queryTokens) const
{
    std::vector<std::pair<size_t, size_t>> rawRanges;
    if (text.empty() || queryTokens.empty())
    {
        return rawRanges;
    }

    std::string lowerText = toLower(text);

    for (const auto& token : queryTokens)
    {
        if (token.empty())
        {
            continue;
        }

        size_t pos = lowerText.find(token);
        while (pos != std::string::npos)
        {
            rawRanges.emplace_back(pos, pos + token.size());
            pos = lowerText.find(token, pos + 1);
        }

        if (token == "brep")
        {
            size_t bPos = lowerText.find("b-rep");
            while (bPos != std::string::npos)
            {
                rawRanges.emplace_back(bPos, bPos + 5);
                bPos = lowerText.find("b-rep", bPos + 1);
            }
        }
    }

    if (rawRanges.empty())
    {
        return rawRanges;
    }

    std::sort(rawRanges.begin(),
              rawRanges.end(),
              [](const std::pair<size_t, size_t>& a, const std::pair<size_t, size_t>& b)
              {
                  if (a.first != b.first)
                  {
                      return a.first < b.first;
                  }
                  return a.second > b.second;
              });

    std::vector<std::pair<size_t, size_t>> merged;
    for (const auto& r : rawRanges)
    {
        if (merged.empty() || merged.back().second < r.first)
        {
            merged.push_back(r);
        }
        else
        {
            merged.back().second = std::max(merged.back().second, r.second);
        }
    }

    return merged;
}

std::vector<std::string> SearchBar::extractContextLines(const std::string& text, const std::vector<std::string>& queryTokens, size_t maxLines) const
{
    std::vector<std::string> lines;
    if (text.empty() || maxLines == 0)
    {
        return lines;
    }

    std::istringstream       stream(text);
    std::string              rawLine;
    std::vector<std::string> allLines;

    while (std::getline(stream, rawLine))
    {
        while (!rawLine.empty() && (rawLine.back() == '\r' || rawLine.back() == ' ' || rawLine.back() == '\t'))
        {
            rawLine.pop_back();
        }
        if (!rawLine.empty())
        {
            if (rawLine.size() > 80)
            {
                rawLine = rawLine.substr(0, 77) + "...";
            }
            allLines.push_back(rawLine);
        }
    }

    if (allLines.empty())
    {
        return lines;
    }

    int firstMatchLine = -1;
    for (size_t i = 0; i < allLines.size(); i++)
    {
        if (!findMatchRanges(allLines[i], queryTokens).empty())
        {
            firstMatchLine = static_cast<int>(i);
            break;
        }
    }

    int startLine = 0;
    if (firstMatchLine >= 0)
    {
        startLine = std::max(0, firstMatchLine - 1);
        if (startLine + static_cast<int>(maxLines) > static_cast<int>(allLines.size()))
        {
            startLine = std::max(0, static_cast<int>(allLines.size()) - static_cast<int>(maxLines));
        }
    }

    for (size_t i = static_cast<size_t>(startLine); i < allLines.size() && lines.size() < maxLines; i++)
    {
        lines.push_back(allLines[i]);
    }

    return lines;
}

std::vector<std::string> SearchBar::tokenizeQuery(const std::string& text) const
{
    std::string spaced = text;
    for (char& c : spaced)
    {
        if (c == '_' || c == '-' || c == '/' || c == '\\' || c == '.' || c == ',')
        {
            c = ' ';
        }
    }

    std::istringstream       stream(spaced);
    std::vector<std::string> tokens;
    std::string              token;

    while (stream >> token)
    {
        tokens.push_back(toLower(token));
    }
    return tokens;
}

std::string SearchBar::toLower(const std::string& input) const
{
    std::string result = input;
    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return result;
}

void SearchBar::executeSelection(size_t index, Defer& defer)
{
    if (index >= results.size())
    {
        return;
    }

    const auto& item   = results[index];
    auto        action = item.getAction();
    if (action)
    {
        defer.defer(action);
    }
    else if (windows)
    {
        if (item.getCategory() == "Command")
        {
            defer.defer([this]() { windows->open("CommandsInfo"); });
        }
        else if (item.getCategory() == "Concept")
        {
            defer.defer([this]() { windows->open("ConceptsInfo"); });
        }
    }

    query.clear();
    lastQuery.clear();
    results.clear();
    selectedIndex = -1;
    isOpen        = false;
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("SearchBar/Construction")
{
    SearchBar searchBar;
}
#endif
