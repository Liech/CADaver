#include "UI/Project/ProjectWindow.h"

#include "Database/Asset.h"
#include "Database/Asset/ProgressBarAsset.h"
#include "Database/AssetComponent.h"
#include "Database/Project.h"
#include "Execution/GatedDatabase.h"
#include "UI/ContextMenu/AssetContextMenuFactory.h"
#include "Windows/Asset/TextNoteEditWindow.h"
#include "Windows/Project/TaskNotificationModal.h"
#include "Windows/Windows.h"

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

void ProjectWindow::render(GatedDatabase& gatedDatabase, Windows& windows)
{
    const auto&          project  = gatedDatabase.get().getProject();
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

        renderMenuBar(project, windows);

        if (project.getAssetCount() == 0)
        {
            ImGui::Spacing();
            ImGui::TextDisabled("No assets loaded");
        }
        else
        {
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
                        ImGui::TextUnformatted(progressAsset->getName().c_str());
                        float p = progressAsset->getProgress();
                        ImGui::ProgressBar(p, ImVec2(-1.0f, 14.0f));

                        std::string popupId = "ProgressBarContext##" + asset->getGuid().toString();
                        if (ImGui::BeginPopupContextItem(popupId.c_str()))
                        {
                            if (ImGui::MenuItem("Cancel"))
                            {
                                progressAsset->cancel();
                                windows.open("TaskNotification");
                                auto modal = std::dynamic_pointer_cast<TaskNotificationModal>(windows.get("TaskNotification"));
                                if (modal)
                                {
                                    modal->setMessage("Operation '" + progressAsset->getOperationName() + "' was canceled.");
                                }
                            }
                            ImGui::EndPopup();
                        }

                        if (progressAsset->isReady())
                        {
                            Polyglot::Node res = progressAsset->getResult();
                            std::string    st  = res["status"].isString() ? res["status"].asString() : "";
                            if (!st.empty() && st != "Success" && st != "Canceled")
                            {
                                windows.open("TaskNotification");
                                auto modal = std::dynamic_pointer_cast<TaskNotificationModal>(windows.get("TaskNotification"));
                                if (modal)
                                {
                                    std::string err = res["error"].isString() ? res["error"].asString() : st;
                                    modal->setMessage("Operation '" + progressAsset->getOperationName() + "' failed: " + err);
                                }
                            }
                        }
                        continue;
                    }
                }

                auto               attachments = asset->getAttachments();
                ImGuiTreeNodeFlags nodeFlags   = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
                if (attachments.empty())
                {
                    nodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
                }

                std::string assetLabel = asset->getName() + " [" + asset->getType() + "]##" + asset->getGuid().toString();
                bool        nodeOpen   = ImGui::TreeNodeEx(assetLabel.c_str(), nodeFlags);

                if (contextMenuFactory)
                {
                    contextMenuFactory->render(gatedDatabase, windows, asset);
                }

                if (nodeOpen && !attachments.empty())
                {
                    for (const auto& comp : attachments)
                    {
                        if (!comp)
                        {
                            continue;
                        }

                        std::string        compLabel = comp->getName() + " (" + comp->getType() + ")##" + comp->getGuid().toString();
                        ImGuiTreeNodeFlags leafFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth;
                        ImGui::TreeNodeEx(compLabel.c_str(), leafFlags);

                        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
                        {
                            if (comp->getType() == "TextNote")
                            {
                                windows.open("TextNoteEdit");
                                auto editWin = std::dynamic_pointer_cast<TextNoteEditWindow>(windows.get("TextNoteEdit"));
                                if (editWin)
                                {
                                    editWin->setTarget(asset->getGuid(), comp->getGuid());
                                }
                            }
                        }

                        std::string compContextId = "ComponentContext##" + comp->getGuid().toString();
                        if (ImGui::BeginPopupContextItem(compContextId.c_str()))
                        {
                            if (comp->getType() == "TextNote" && ImGui::MenuItem("Edit Note..."))
                            {
                                windows.open("TextNoteEdit");
                                auto editWin = std::dynamic_pointer_cast<TextNoteEditWindow>(windows.get("TextNoteEdit"));
                                if (editWin)
                                {
                                    editWin->setTarget(asset->getGuid(), comp->getGuid());
                                }
                            }

                            if (ImGui::MenuItem("Remove Component"))
                            {
                                Polyglot::Node command = {
                                    {          "type",           "RemoveComponent" },
                                    {       "version",                           1 },
                                    { "componentGuid",  comp->getGuid().toString() },
                                    {     "assetGuid", asset->getGuid().toString() }
                                };
                                gatedDatabase.execute(command);
                            }
                            ImGui::EndPopup();
                        }
                    }
                    ImGui::TreePop();
                }
            }
        }
    }
    ImGui::End();
}

void ProjectWindow::renderMenuBar(const Project& project, Windows& windows)
{
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Clear Project...", nullptr, false, project.getAssetCount() > 0))
            {
                windows.open("ClearProject");
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
}
