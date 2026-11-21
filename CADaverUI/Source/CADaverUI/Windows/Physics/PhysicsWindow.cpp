#include "Windows/Physics/PhysicsWindow.h"

#include "Execution/Defer.h"
#include "Execution/GatedDatabase.h"

#include <imgui.h>

PhysicsWindow::PhysicsWindow() {}

PhysicsWindow::~PhysicsWindow() {}

std::string PhysicsWindow::getType() const
{
    return "Physics";
}

std::string PhysicsWindow::getDescription() const
{
    return "Window: Physics\n"
           "Title: Physics Simulation\n"
           "Purpose: Controls real-time physics simulation playback with Play and Stop buttons that continuously dispatch mergeable StepPhysics commands.\n\n"
           "Parameters:\n"
           "  - Play / Stop: Starts or stops advancing the physics simulation over time\n\n"
           "Dispatched Commands: StepPhysics";
}

void PhysicsWindow::render()
{
    if (!openState)
    {
        playing = false;
        return;
    }

    if (!gatedDatabase || !defer)
    {
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(260.0f, 120.0f), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Physics Simulation", &openState))
    {
        handleEscapeClose();

        ImGui::Text("Elapsed Time: %.2f s", elapsedTime);
        ImGui::Spacing();

        if (!playing)
        {
            if (ImGui::Button("Play", ImVec2(100.0f, 0.0f)))
            {
                playing          = true;
                currentMergeGuid = CADacomb::GUID::random();
                elapsedTime      = 0.0;
            }
        }
        else
        {
            if (ImGui::Button("Stop", ImVec2(100.0f, 0.0f)))
            {
                playing = false;
            }
        }

        ImGui::SameLine();
        if (ImGui::Button("Close", ImVec2(90.0f, 0.0f)))
        {
            playing = false;
            close();
        }

        if (playing)
        {
            const double   stepDelta = 1.0 / 60.0;
            CADacomb::GUID mergeGuid = currentMergeGuid;
            elapsedTime += stepDelta;
            defer->defer(
              [this, stepDelta, mergeGuid]()
              {
                  Polyglot::Node command = {
                      {      "type",        "StepPhysics" },
                      {   "version",                    1 },
                      { "deltaTime",            stepDelta },
                      { "mergeGuid", mergeGuid.toString() }
                  };
                  gatedDatabase->execute(command);
              });
        }
    }
    ImGui::End();
}

bool PhysicsWindow::isPlaying() const
{
    return playing;
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("PhysicsWindow/TypeAndDescription")
{
    PhysicsWindow window;
    CHECK(window.getType() == "Physics");
    CHECK_FALSE(window.isPlaying());
    CHECK(window.getDescription().find("Window: Physics") != std::string::npos);
    CHECK(window.getDescription().find("StepPhysics") != std::string::npos);
}
#endif
