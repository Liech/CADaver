#include "UI/Component/ModalProgressBar.h"

#include "CADacomb/Util/ProgressBar.h"

#include <imgui.h>

ModalProgressBar::ModalProgressBar() {}

ModalProgressBar::~ModalProgressBar() {}

void ModalProgressBar::setProgress(float progressInput, const std::string& messageInput)
{
    progress.store(progressInput, std::memory_order_relaxed);
    {
        std::lock_guard<std::mutex> lock(messageMutex);
        message = messageInput;
    }
}

float ModalProgressBar::getProgress() const
{
    return progress.load(std::memory_order_relaxed);
}

std::string ModalProgressBar::getMessage() const
{
    std::lock_guard<std::mutex> lock(messageMutex);
    return message;
}

void ModalProgressBar::setGuid(const CADacomb::GUID& newGuid)
{
    guid = newGuid;
}

const CADacomb::GUID& ModalProgressBar::getGuid() const
{
    return guid;
}

void ModalProgressBar::reset()
{
    progress.store(0.0f, std::memory_order_relaxed);
    {
        std::lock_guard<std::mutex> lock(messageMutex);
        message = "";
    }
    guid        = CADacomb::GUID();
    shouldClose = false;
}

void ModalProgressBar::close()
{
    shouldClose = true;
}

void ModalProgressBar::render(const std::string& title)
{
    if (shouldClose)
    {
        ImGui::CloseCurrentPopup();
        shouldClose = false;
        return;
    }

    ImGui::OpenPopup(title.c_str());

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal(title.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
    {
        std::vector<CADacomb::ProgressBarStatus> statuses;
        if (!guid.toString().empty())
        {
            statuses = CADacomb::ProgressBar::getProgress(guid);
        }

        if (!statuses.empty())
        {
            for (const auto& status : statuses)
            {
                if (!status.message.empty())
                {
                    ImGui::Text("%s", status.message.c_str());
                }
                float ratio = (status.max > 0) ? (static_cast<float>(status.current) / static_cast<float>(status.max)) : 0.0f;
                ImGui::ProgressBar(ratio, ImVec2(360.0f, 20.0f));
                ImGui::Spacing();
            }

            if (ImGui::Button("Cancel", ImVec2(120.0f, 0.0f)))
            {
                CADacomb::ProgressBar::cancel(guid);
            }
        }
        else
        {
            std::string currentMessage = getMessage();
            if (!currentMessage.empty())
            {
                ImGui::Text("%s", currentMessage.c_str());
            }
            ImGui::ProgressBar(getProgress(), ImVec2(360.0f, 20.0f));
        }

        ImGui::EndPopup();
    }
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("ModalProgressBar/InitialState")
{
    ModalProgressBar modal;
    CHECK(modal.getProgress() == 0.0f);
    CHECK(modal.getMessage().empty());
    CHECK(modal.getGuid().toString().empty());
}

TEST_CASE("ModalProgressBar/SetAndReset")
{
    ModalProgressBar modal;
    CADacomb::GUID   guid = CADacomb::GUID::random();
    modal.setGuid(guid);
    modal.setProgress(0.5f, "Working...");

    CHECK(modal.getGuid() == guid);
    CHECK(modal.getProgress() == 0.5f);
    CHECK(modal.getMessage() == "Working...");

    modal.reset();
    CHECK(modal.getProgress() == 0.0f);
    CHECK(modal.getMessage().empty());
    CHECK(modal.getGuid().toString().empty());
}
#endif
