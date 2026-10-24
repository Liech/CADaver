#include "ProjectWindow.h"

#include "Asset.h"
#include "Project.h"

#include <imgui.h>

ProjectWindow::ProjectWindow() {}

ProjectWindow::~ProjectWindow() {}

float ProjectWindow::getWidth() const
{
    return width;
}

void ProjectWindow::setWidth(float newWidth)
{
    width = newWidth;
}

void ProjectWindow::render(Project& project)
{
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(width, viewport->WorkSize.y), ImGuiCond_Always);

    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus;

    if (ImGui::Begin("Project", nullptr, windowFlags))
    {
        if (project.getAssetCount() == 0)
        {
            ImGui::Spacing();
            ImGui::TextDisabled("No assets in project.");
            ImGui::TextDisabled("Use File -> Load -> STL or STEP");
        }
        else
        {
            if (ImGui::BeginTable("AssetTable", 2, ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingStretchProp))
            {
                ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 110.0f);
                ImGui::TableHeadersRow();

                const auto& assets        = project.getAssets();
                int         selectedIndex = project.getSelectedIndex();

                for (size_t i = 0; i < assets.size(); i++)
                {
                    const auto& asset = assets[i];
                    if (!asset)
                    {
                        continue;
                    }

                    ImGui::TableNextRow();

                    ImGui::TableSetColumnIndex(0);
                    bool        isSelected = (selectedIndex == static_cast<int>(i));
                    std::string label      = asset->getName() + "##" + std::to_string(i);
                    if (ImGui::Selectable(label.c_str(), isSelected, ImGuiSelectableFlags_SpanAllColumns))
                    {
                        project.setSelectedIndex(static_cast<int>(i));
                    }

                    ImGui::TableSetColumnIndex(1);
                    ImGui::TextUnformatted(asset->getTypeName().c_str());
                }
                ImGui::EndTable();
            }
        }
    }
    ImGui::End();
}
