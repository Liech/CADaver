#include "UI/Preferences/UndoPreferencesView.h"

#include "Commands/Undo/UndoRecord.h"
#include "Commands/Undo/UndoStack.h"
#include "Execution/Defer.h"
#include "Execution/GatedDatabase.h"

#include <cstdio>
#include <imgui.h>

UndoPreferencesView::UndoPreferencesView() {}

UndoPreferencesView::~UndoPreferencesView() {}

bool UndoPreferencesView::render(const UndoStack& undoStack)
{
    bool modified = false;

    ImGui::Text("Capacity & Memory Metrics");
    ImGui::Separator();

    size_t totalBytes   = undoStack.getTotalMemoryBytes();
    size_t maxBytes     = undoStack.getMaxMemoryBytes();
    size_t maxSteps     = undoStack.getMaxSteps();
    size_t currentSteps = undoStack.getUndoHistory().size();

    ImGui::Text("Steps Limit: %zu steps (Current: %zu)", maxSteps, currentSteps);
    ImGui::Text("Memory Limit: %zu MB", maxBytes / (1024ULL * 1024ULL));

    float fraction = maxBytes > 0 ? static_cast<float>(totalBytes) / static_cast<float>(maxBytes) : 0.0f;
    if (fraction > 1.0f)
    {
        fraction = 1.0f;
    }

    std::string memoryOverlay = formatByteSize(totalBytes) + " / " + formatByteSize(maxBytes);
    ImGui::ProgressBar(fraction, ImVec2(-1.0f, 0.0f), memoryOverlay.c_str());

    ImGui::Spacing();
    ImGui::BeginDisabled(!undoStack.canUndo());
    if (ImGui::Button("Undo"))
    {
        Defer::getInstance().defer(
          []()
          {
              GatedDatabase::getInstance().execute({
                {    "type", "Undo" },
                { "version",      1 }
              });
          });
        modified = true;
    }
    ImGui::EndDisabled();

    ImGui::SameLine();
    ImGui::BeginDisabled(!undoStack.canRedo());
    if (ImGui::Button("Redo"))
    {
        Defer::getInstance().defer(
          []()
          {
              GatedDatabase::getInstance().execute({
                {    "type", "Redo" },
                { "version",      1 }
              });
          });
        modified = true;
    }
    ImGui::EndDisabled();

    ImGui::Spacing();
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Undo History", ImGuiTreeNodeFlags_DefaultOpen))
    {
        const auto& undoHistory = undoStack.getUndoHistory();
        if (undoHistory.empty())
        {
            ImGui::TextDisabled("No undo history recorded yet.");
        }
        else
        {
            if (ImGui::BeginTable("UndoHistoryTable", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY, ImVec2(0.0f, 120.0f)))
            {
                ImGui::TableSetupColumn("#", ImGuiTableColumnFlags_WidthFixed, 30.0f);
                ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed, 75.0f);
                ImGui::TableHeadersRow();

                for (size_t i = undoHistory.size(); i > 0; i--)
                {
                    size_t      idx    = i - 1;
                    const auto& record = undoHistory[idx];
                    if (record)
                    {
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("%zu", idx + 1);

                        ImGui::TableSetColumnIndex(1);
                        ImGui::TextUnformatted(record->getDescription().c_str());

                        ImGui::TableSetColumnIndex(2);
                        std::string sizeStr = formatByteSize(record->getSizeInBytes());
                        ImGui::TextUnformatted(sizeStr.c_str());
                    }
                }
                ImGui::EndTable();
            }
        }
    }

    if (ImGui::CollapsingHeader("Redo History"))
    {
        const auto& redoHistory = undoStack.getRedoHistory();
        if (redoHistory.empty())
        {
            ImGui::TextDisabled("No redo history available.");
        }
        else
        {
            if (ImGui::BeginTable("RedoHistoryTable", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY, ImVec2(0.0f, 90.0f)))
            {
                ImGui::TableSetupColumn("#", ImGuiTableColumnFlags_WidthFixed, 30.0f);
                ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed, 75.0f);
                ImGui::TableHeadersRow();

                for (size_t i = redoHistory.size(); i > 0; i--)
                {
                    size_t      idx    = i - 1;
                    const auto& record = redoHistory[idx];
                    if (record)
                    {
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("%zu", idx + 1);

                        ImGui::TableSetColumnIndex(1);
                        ImGui::TextUnformatted(record->getDescription().c_str());

                        ImGui::TableSetColumnIndex(2);
                        std::string sizeStr = formatByteSize(record->getSizeInBytes());
                        ImGui::TextUnformatted(sizeStr.c_str());
                    }
                }
                ImGui::EndTable();
            }
        }
    }

    return modified;
}

std::string UndoPreferencesView::formatByteSize(size_t bytes) const
{
    if (bytes >= 1024ULL * 1024ULL * 1024ULL)
    {
        double gb = static_cast<double>(bytes) / (1024.0 * 1024.0 * 1024.0);
        char   buffer[64];
        std::snprintf(buffer, sizeof(buffer), "%.2f GB", gb);
        return std::string(buffer);
    }
    if (bytes >= 1024ULL * 1024ULL)
    {
        double mb = static_cast<double>(bytes) / (1024.0 * 1024.0);
        char   buffer[64];
        std::snprintf(buffer, sizeof(buffer), "%.2f MB", mb);
        return std::string(buffer);
    }
    if (bytes >= 1024ULL)
    {
        double kb = static_cast<double>(bytes) / 1024.0;
        char   buffer[64];
        std::snprintf(buffer, sizeof(buffer), "%.1f KB", kb);
        return std::string(buffer);
    }
    return std::to_string(bytes) + " B";
}
