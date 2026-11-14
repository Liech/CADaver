#include "UI/Project/ProjectWindow.h"

#include "CADacomb/Util/GUID.h"
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
    Polyglot::Node response   = gatedDatabase.execute({
      {    "type", "GetAssets" },
      { "version",           1 }
    });
    nlohmann::json assetsJson = response.toJson().value("assets", nlohmann::json::array());

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

        renderMenuBar(!assetsJson.empty(), windows);

        if (assetsJson.empty())
        {
            ImGui::Spacing();
            ImGui::TextDisabled("No assets loaded");
        }
        else
        {
            for (const auto& assetItem : assetsJson)
            {
                CADacomb::GUID assetGuid(assetItem.value("assetGuid", ""));
                std::string    assetName = assetItem.value("name", "");
                std::string    assetType = assetItem.value("type", "");

                if (assetType == "Progress")
                {
                    std::string operationName = assetItem.value("operationName", assetName);
                    float       p             = assetItem.value("progress", 0.0f);
                    bool        isReady       = assetItem.value("isReady", false);

                    ImGui::TextUnformatted(assetName.c_str());
                    ImGui::ProgressBar(p, ImVec2(-1.0f, 14.0f));

                    std::string popupId = "ProgressBarContext##" + assetGuid.toString();
                    if (ImGui::BeginPopupContextItem(popupId.c_str()))
                    {
                        if (ImGui::MenuItem("Cancel"))
                        {
                            gatedDatabase.execute({
                              {      "type",         "CancelTask" },
                              {   "version",                    1 },
                              { "assetGuid", assetGuid.toString() }
                            });
                            windows.open("TaskNotification");
                            auto modal = std::dynamic_pointer_cast<TaskNotificationModal>(windows.get("TaskNotification"));
                            if (modal)
                            {
                                modal->setMessage("Operation '" + operationName + "' was canceled.");
                            }
                        }
                        ImGui::EndPopup();
                    }

                    if (isReady)
                    {
                        std::string st = assetItem.value("taskStatus", "");
                        if (!st.empty() && st != "Success" && st != "Canceled")
                        {
                            windows.open("TaskNotification");
                            auto modal = std::dynamic_pointer_cast<TaskNotificationModal>(windows.get("TaskNotification"));
                            if (modal)
                            {
                                std::string err = assetItem.value("taskError", "");
                                if (err.empty())
                                {
                                    err = st;
                                }
                                modal->setMessage("Operation '" + operationName + "' failed: " + err);
                            }
                        }
                    }
                    continue;
                }

                nlohmann::json     components = assetItem.value("components", nlohmann::json::array());
                ImGuiTreeNodeFlags nodeFlags  = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
                if (components.empty())
                {
                    nodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
                }

                std::string assetLabel = assetName + " [" + assetType + "]##" + assetGuid.toString();
                bool        nodeOpen   = ImGui::TreeNodeEx(assetLabel.c_str(), nodeFlags);

                if (contextMenuFactory)
                {
                    contextMenuFactory->render(gatedDatabase, windows, assetGuid, assetType);
                }

                if (nodeOpen && !components.empty())
                {
                    for (const auto& compItem : components)
                    {
                        CADacomb::GUID compGuid(compItem.value("componentGuid", ""));
                        std::string    compName = compItem.value("name", "");
                        std::string    compType = compItem.value("type", "");

                        std::string        compLabel = compName + " (" + compType + ")##" + compGuid.toString();
                        ImGuiTreeNodeFlags leafFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth;
                        ImGui::TreeNodeEx(compLabel.c_str(), leafFlags);

                        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
                        {
                            if (compType == "TextNote")
                            {
                                windows.open("TextNoteEdit");
                                auto editWin = std::dynamic_pointer_cast<TextNoteEditWindow>(windows.get("TextNoteEdit"));
                                if (editWin)
                                {
                                    editWin->setTarget(assetGuid, compGuid);
                                }
                            }
                        }

                        std::string compContextId = "ComponentContext##" + compGuid.toString();
                        if (ImGui::BeginPopupContextItem(compContextId.c_str()))
                        {
                            if (compType == "TextNote" && ImGui::MenuItem("Edit Note..."))
                            {
                                windows.open("TextNoteEdit");
                                auto editWin = std::dynamic_pointer_cast<TextNoteEditWindow>(windows.get("TextNoteEdit"));
                                if (editWin)
                                {
                                    editWin->setTarget(assetGuid, compGuid);
                                }
                            }

                            if (ImGui::MenuItem("Remove Component"))
                            {
                                Polyglot::Node command = {
                                    {          "type",    "RemoveComponent" },
                                    {       "version",                    1 },
                                    { "componentGuid",  compGuid.toString() },
                                    {     "assetGuid", assetGuid.toString() }
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

void ProjectWindow::renderMenuBar(bool hasAssets, Windows& windows)
{
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Clear Project...", nullptr, false, hasAssets))
            {
                windows.open("ClearProject");
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
}
