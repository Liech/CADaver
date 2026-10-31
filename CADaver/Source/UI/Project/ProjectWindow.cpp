#include "UI/Project/ProjectWindow.h"

#include "Commands/Defer.h"
#include "Commands/GatedDatabase.h"
#include "Database/Asset.h"
#include "Database/Asset/ProgressBarAsset.h"
#include "Database/Project.h"
#include "UI/ContextMenu/AssetContextMenuFactory.h"

#include <imgui.h>

ProjectWindow::ProjectWindow()
  : contextMenuFactory(std::make_unique<AssetContextMenuFactory>())
{
}

ProjectWindow::~ProjectWindow() {}

float ProjectWindow::getWidth() const
{
    return width;
}

void ProjectWindow::setWidth(float newWidth)
{
    width = newWidth;
}

void ProjectWindow::render(const Project& project)
{
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos, ImGuiCond_Always);

    float minWidth = 150.0f;
    float maxWidth = viewport->WorkSize.x * 0.8f;
    ImGui::SetNextWindowSizeConstraints(ImVec2(minWidth, viewport->WorkSize.y), ImVec2(maxWidth, viewport->WorkSize.y));
    ImGui::SetNextWindowSize(ImVec2(width, viewport->WorkSize.y), ImGuiCond_FirstUseEver);

    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_MenuBar;

    if (ImGui::Begin("Project", nullptr, windowFlags))
    {
        width = ImGui::GetWindowWidth();

        renderMenuBar(project);

        if (project.getAssetCount() == 0)
        {
            ImGui::Spacing();
        }
        else
        {
            if (ImGui::BeginTable("AssetTable", 2, ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingStretchProp))
            {
                ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 110.0f);
                ImGui::TableHeadersRow();

                for (const auto& asset : project.getAssets())
                {
                    if (!asset)
                    {
                        continue;
                    }

                    if (asset->getType() == "Progress")
                    {
                        auto progressAsset = std::dynamic_pointer_cast<const ProgressBarAsset>(asset);
                        if (progressAsset)
                        {
                            ImGui::TableNextRow();

                            ImGui::TableSetColumnIndex(0);
                            ImGui::TextUnformatted(progressAsset->getName().c_str());
                            float p = progressAsset->getProgress();
                            ImGui::ProgressBar(p, ImVec2(-1.0f, 14.0f));

                            std::string popupId = "ProgressBarContext##" + asset->getGuid().toString();
                            if (ImGui::BeginPopupContextItem(popupId.c_str()))
                            {
                                if (ImGui::MenuItem("Cancel"))
                                {
                                    progressAsset->cancel();
                                    errorMessage   = "Operation '" + progressAsset->getOperationName() + "' was canceled.";
                                    openErrorModal = true;
                                }
                                ImGui::EndPopup();
                            }

                            if (progressAsset->isReady())
                            {
                                auto        res = progressAsset->getResult();
                                std::string st  = res.value("status", "");
                                if (!st.empty() && st != "Success" && st != "Canceled")
                                {
                                    errorMessage   = "Operation '" + progressAsset->getOperationName() + "' failed: " + res.value("error", st);
                                    openErrorModal = true;
                                }
                            }

                            ImGui::TableSetColumnIndex(1);
                            ImGui::TextUnformatted("Processing...");
                            continue;
                        }
                    }

                    ImGui::TableNextRow();

                    ImGui::TableSetColumnIndex(0);
                    std::string label = asset->getName() + "##" + asset->getGuid().toString();
                    ImGui::Selectable(label.c_str(), false, ImGuiSelectableFlags_SpanAllColumns);

                    if (contextMenuFactory)
                    {
                        contextMenuFactory->render(asset);
                    }

                    ImGui::TableSetColumnIndex(1);
                    ImGui::TextUnformatted(asset->getType().c_str());
                }
                ImGui::EndTable();
            }
        }

        if (contextMenuFactory)
        {
            contextMenuFactory->renderCloseModal();
            contextMenuFactory->renderConversionModals();
        }

        renderClearModal();
        renderErrorModal();
    }
    ImGui::End();
}

void ProjectWindow::renderMenuBar(const Project& project)
{
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Clear Project...", nullptr, false, project.getAssetCount() > 0))
            {
                openClearModal = true;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
}

void ProjectWindow::renderClearModal()
{
    if (openClearModal)
    {
        ImGui::OpenPopup("Clear Project##Modal");
        openClearModal = false;
    }

    if (ImGui::BeginPopupModal("Clear Project##Modal", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Are you sure you want to clear the project?");
        ImGui::TextDisabled("All assets will be removed from the project.");
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::Button("Yes", ImVec2(120.0f, 0.0f)))
        {
            Defer::getInstance().defer(
              []()
              {
                  GatedDatabase::getInstance().execute(
                    {
                      {    "type", "ClearProject" },
                      { "version",              1 }
                  },
                    true);
              });
            ImGui::CloseCurrentPopup();
        }
        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120.0f, 0.0f)))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void ProjectWindow::renderErrorModal()
{
    if (openErrorModal)
    {
        ImGui::OpenPopup("Task Notification##Modal");
        openErrorModal = false;
    }

    if (ImGui::BeginPopupModal("Task Notification##Modal", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::TextUnformatted(errorMessage.c_str());
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::Button("OK", ImVec2(120.0f, 0.0f)))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}
