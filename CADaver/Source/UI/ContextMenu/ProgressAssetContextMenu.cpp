#include "UI/ContextMenu/ProgressAssetContextMenu.h"

#include "Execution/GatedDatabase.h"
#include "Windows/Project/ProgressWindow.h"
#include "Windows/Project/TaskNotificationModal.h"
#include "Windows/Windows.h"

#include <imgui.h>
#include <memory>
#include <nlohmann/json.hpp>

ProgressAssetContextMenu::ProgressAssetContextMenu() {}

ProgressAssetContextMenu::~ProgressAssetContextMenu() {}

void ProgressAssetContextMenu::render(GatedDatabase& gatedDatabase, Windows& windows, const CADacomb::GUID& assetGuid)
{
    if (ImGui::MenuItem("Show Progress..."))
    {
        windows.open("Progress");
        auto window = std::dynamic_pointer_cast<ProgressWindow>(windows.get("Progress"));
        if (window)
        {
            window->setTargetAsset(assetGuid);
        }
    }

    if (ImGui::MenuItem("Cancel"))
    {
        Polyglot::Node stateResponse = gatedDatabase.execute({
          {      "type",       "GetTaskState" },
          {   "version",                    1 },
          { "assetGuid", assetGuid.toString() }
        });
        std::string    operationName = stateResponse.toJson().value("operationName", "");

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
}
