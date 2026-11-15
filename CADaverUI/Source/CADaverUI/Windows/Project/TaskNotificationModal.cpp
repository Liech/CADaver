#include "Windows/Project/TaskNotificationModal.h"

#include <imgui.h>

TaskNotificationModal::TaskNotificationModal() {}

TaskNotificationModal::~TaskNotificationModal() {}

std::string TaskNotificationModal::getType() const
{
    return "TaskNotification";
}

std::string TaskNotificationModal::getDescription() const
{
    return "Window: TaskNotification\n"
           "Title: Task Notification\n"
           "Purpose: Displays status notifications, cancellation messages, or error details from background asynchronous tasks.\n\n"
           "Parameters:\n"
           "  - Message: Notification or error text to display\n\n"
           "Dispatched Command: None (notification window)";
}

const std::string& TaskNotificationModal::getMessage() const
{
    return message;
}

void TaskNotificationModal::setMessage(const std::string& messageInput)
{
    message = messageInput;
}

void TaskNotificationModal::render()
{
    if (!openState)
    {
        return;
    }

    ImGui::SetNextWindowSizeConstraints(ImVec2(360.0f, 0.0f), ImVec2(760.0f, 400.0f));
    if (ImGui::Begin("Task Notification##TaskNotificationWindow", &openState, ImGuiWindowFlags_AlwaysAutoResize))
    {
        const std::string displayMessage = message.empty() ? "An unknown error occurred." : message;
        ImGui::TextWrapped("%s", displayMessage.c_str());
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::Button("OK", ImVec2(120.0f, 0.0f)))
        {
            close();
        }

        ImGui::SameLine();
        if (ImGui::Button("Copy", ImVec2(120.0f, 0.0f)))
        {
            ImGui::SetClipboardText(displayMessage.c_str());
        }
    }
    ImGui::End();
}
