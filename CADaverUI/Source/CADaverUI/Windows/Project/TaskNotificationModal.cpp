#include "Windows/Project/TaskNotificationModal.h"

#include <imgui.h>

TaskNotificationModal::TaskNotificationModal() {}

TaskNotificationModal::~TaskNotificationModal() {}

std::string TaskNotificationModal::getType() const
{
    return "TaskNotificationModal";
}

std::string TaskNotificationModal::getDescription() const
{
    return "Window: TaskNotificationModal\n"
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

    if (ImGui::Begin("Task Notification##TaskNotificationWindow", &openState, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::TextUnformatted(message.c_str());
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::Button("OK", ImVec2(120.0f, 0.0f)))
        {
            close();
        }
    }
    ImGui::End();
}
