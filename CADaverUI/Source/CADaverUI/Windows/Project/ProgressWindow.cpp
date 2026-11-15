#include "Windows/Project/ProgressWindow.h"

#include "Execution/GatedDatabase.h"

#include <imgui.h>
#include <nlohmann/json.hpp>

ProgressWindow::ProgressWindow()
{
}

ProgressWindow::~ProgressWindow()
{
}

std::string ProgressWindow::getType() const
{
    return "Progress";
}

std::string ProgressWindow::getDescription() const
{
    return "Window: Progress\n"
           "Title: Progress\n"
           "Purpose: Non-modal window displaying all nested progress bar tiers of a running ProgressBarAsset.\n\n"
           "Parameters:\n"
           "  - Target Asset: ProgressBarAsset GUID to monitor\n\n"
           "Dispatched Commands: GetTaskState, CancelTask";
}

void ProgressWindow::render()
{
    if (!openState || !gatedDatabase)
    {
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(460.0f, 240.0f), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Progress", &openState))
    {
        handleEscapeClose();

        if (targetAssetGuid.toString().empty())
        {
            if (ImGui::Button("Close", ImVec2(120.0f, 0.0f)))
            {
                close();
            }
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::TextDisabled("No progress task selected.");
        }
        else
        {
            Polyglot::Node response = gatedDatabase->execute({
              {      "type",               "GetTaskState" },
              {   "version",                            1 },
              { "assetGuid", targetAssetGuid.toString() }
            });
            nlohmann::json json = response.toJson();

            if (json.value("type", "") == "Error")
            {
                if (ImGui::Button("Close", ImVec2(120.0f, 0.0f)))
                {
                    close();
                }
                ImGui::Separator();
                ImGui::Spacing();
                ImGui::TextDisabled("Task finished or no longer active.");
            }
            else
            {
                std::string    operationName = json.value("operationName", "");
                nlohmann::json tiers         = json.value("tiers", nlohmann::json::array());

                if (ImGui::Button("Cancel Task", ImVec2(120.0f, 0.0f)))
                {
                    gatedDatabase->execute({
                      {      "type",               "CancelTask" },
                      {   "version",                          1 },
                      { "assetGuid", targetAssetGuid.toString() }
                    });
                }
                ImGui::SameLine();
                if (ImGui::Button("Close", ImVec2(120.0f, 0.0f)))
                {
                    close();
                }

                ImGui::Spacing();
                ImGui::Text("Operation: %s", operationName.c_str());
                ImGui::Separator();
                ImGui::Spacing();

                if (tiers.empty())
                {
                    float progress = json.value("progress", 0.0f);
                    ImGui::TextDisabled("Waiting for progress...");
                    ImGui::ProgressBar(progress, ImVec2(-1.0f, 16.0f));
                }
                else
                {
                    for (size_t i = 0; i < tiers.size(); i++)
                    {
                        const auto& tier         = tiers[i];
                        std::string message      = tier.value("message", "");
                        size_t      current      = tier.value("current", static_cast<size_t>(0));
                        size_t      max          = tier.value("max", static_cast<size_t>(0));
                        float       tierProgress = tier.value("progress", 0.0f);

                        if (i > 0)
                        {
                            ImGui::Indent(static_cast<float>(i) * 14.0f);
                        }

                        if (max > 0)
                        {
                            ImGui::Text("%s (%zu / %zu)", message.c_str(), current, max);
                        }
                        else
                        {
                            ImGui::TextUnformatted(message.c_str());
                        }
                        ImGui::ProgressBar(tierProgress, ImVec2(-1.0f, 16.0f));

                        if (i > 0)
                        {
                            ImGui::Unindent(static_cast<float>(i) * 14.0f);
                        }
                    }
                }
            }
        }
    }
    ImGui::End();
}

void ProgressWindow::setTargetAsset(const CADacomb::GUID& assetGuid)
{
    targetAssetGuid = assetGuid;
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("ProgressWindow/TypeAndOpenState")
{
    ProgressWindow window;
    CHECK(window.getType() == "Progress");
    CHECK(!window.isOpen());
    window.open();
    CHECK(window.isOpen());
    window.close();
    CHECK(!window.isOpen());
}
#endif
