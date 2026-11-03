#include "Windows/Console/ConsoleWindow.h"

#include "Windows/Console/ConsoleStreamBuffer.h"

#include <imgui.h>
#include <iostream>

ConsoleWindow::ConsoleWindow(bool autoAttach)
{
    if (autoAttach)
    {
        attachStreams();
    }
}

ConsoleWindow::~ConsoleWindow()
{
    detachStreams();
}

std::string ConsoleWindow::getType() const
{
    return "Console";
}

std::string ConsoleWindow::getDescription() const
{
    return "Window: Console\n"
           "Title: Console\n"
           "Purpose: Displays runtime logs, execution output, and formatted error tracebacks.\n\n"
           "Parameters:\n"
           "  - Clear: Button clearing console log history\n"
           "  - Auto-scroll: Checkbox toggling automatic scrolling on new output\n"
           "  - Copy All: Button copying all console messages to the clipboard\n\n"
           "Dispatched Command: None (log viewing window)";
}

void ConsoleWindow::log(const std::string& message)
{
    std::lock_guard<std::mutex> lock(messagesMutex);
    messages.push_back({ message, false });
}

void ConsoleWindow::logError(const std::string& message)
{
    std::lock_guard<std::mutex> lock(messagesMutex);
    messages.push_back({ message, true });
}

void ConsoleWindow::clear()
{
    std::lock_guard<std::mutex> lock(messagesMutex);
    messages.clear();
}

void ConsoleWindow::attachStreams()
{
    if (!coutBuffer)
    {
        coutBuffer = std::make_unique<ConsoleStreamBuffer>(std::cout, [this](const std::string& line) { log(line); });
    }

    if (!cerrBuffer)
    {
        cerrBuffer = std::make_unique<ConsoleStreamBuffer>(std::cerr, [this](const std::string& line) { logError(line); });
    }
}

void ConsoleWindow::detachStreams()
{
    coutBuffer.reset();
    cerrBuffer.reset();
}

const std::vector<std::pair<std::string, bool>> ConsoleWindow::getMessages() const
{
    std::lock_guard<std::mutex> lock(messagesMutex);
    return messages;
}

size_t ConsoleWindow::getMessageCount() const
{
    std::lock_guard<std::mutex> lock(messagesMutex);
    return messages.size();
}

void ConsoleWindow::renderControls()
{
    if (ImGui::Button("Clear"))
    {
        clear();
    }

    ImGui::SameLine();
    ImGui::Checkbox("Auto-scroll", &autoScroll);

    ImGui::SameLine();
    if (ImGui::Button("Copy All"))
    {
        std::string                 allText;
        std::lock_guard<std::mutex> lock(messagesMutex);
        for (const auto& entry : messages)
        {
            allText += entry.first + "\n";
        }
        ImGui::SetClipboardText(allText.c_str());
    }
}

void ConsoleWindow::renderLogs()
{
    if (ImGui::BeginChild("##ConsoleScrollRegion", ImVec2(0.0f, 0.0f), false, ImGuiWindowFlags_HorizontalScrollbar))
    {
        std::lock_guard<std::mutex> lock(messagesMutex);
        for (const auto& entry : messages)
        {
            if (entry.second)
            {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.35f, 0.35f, 1.0f));
            }
            else
            {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.85f, 0.85f, 0.85f, 1.0f));
            }

            ImGui::TextUnformatted(entry.first.c_str());
            ImGui::PopStyleColor();
        }

        if (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        {
            ImGui::SetScrollHereY(1.0f);
        }
    }
    ImGui::EndChild();
}

void ConsoleWindow::render()
{
    if (!openState)
    {
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(600.0f, 400.0f), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Console", &openState))
    {
        handleEscapeClose();
        renderControls();
        ImGui::Separator();
        renderLogs();
    }
    ImGui::End();
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("ConsoleWindow/DefaultState")
{
    ConsoleWindow console(false);
    CHECK(console.getType() == "Console");
    CHECK(console.getMessageCount() == 0);
    CHECK(console.getMessages().empty());
}

TEST_CASE("ConsoleWindow/LogAndClear")
{
    ConsoleWindow console(false);
    console.log("Normal message");
    CHECK(console.getMessageCount() == 1);
    CHECK(console.getMessages()[0].first == "Normal message");
    CHECK(console.getMessages()[0].second == false);

    console.clear();
    CHECK(console.getMessageCount() == 0);
}

TEST_CASE("ConsoleWindow/LogError")
{
    ConsoleWindow console(false);
    console.logError("Fatal error occurred");
    CHECK(console.getMessageCount() == 1);
    CHECK(console.getMessages()[0].first == "Fatal error occurred");
    CHECK(console.getMessages()[0].second == true);
}

TEST_CASE("ConsoleWindow/StreamRedirectionCout")
{
    ConsoleWindow console(false);
    console.attachStreams();
    std::cout << "Redirected via cout" << std::endl;
    console.detachStreams();

    REQUIRE(console.getMessageCount() >= 1);
    CHECK(console.getMessages().back().first == "Redirected via cout");
    CHECK(console.getMessages().back().second == false);
}

TEST_CASE("ConsoleWindow/StreamRedirectionCerr")
{
    ConsoleWindow console(false);
    console.attachStreams();
    std::cerr << "Redirected via cerr\n";
    console.detachStreams();

    REQUIRE(console.getMessageCount() >= 1);
    CHECK(console.getMessages().front().first == "Redirected via cerr");
    CHECK(console.getMessages().front().second == true);
}
#endif
