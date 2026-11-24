#include "Time/TimeControls.h"

#include "Execution/Defer.h"
#include "Execution/GatedDatabase.h"

#include <format>
#include <imgui.h>
#include <string>

TimeControls::TimeControls() {}

TimeControls::~TimeControls() {}

void TimeControls::render(GatedDatabase& gatedDatabase, Defer& defer)
{
    float frameH = ImGui::GetFrameHeight();

    if (ImGui::Button("##TimePlayPause", ImVec2(frameH, frameH)))
    {
        toggle();
    }

    renderIcon();

    if (ImGui::IsItemHovered())
    {
        double      currentTime = queryCurrentTime(gatedDatabase);
        std::string tooltip     = std::format("{}\nTime: {:.2f} s", playing ? "Pause" : "Play", currentTime);
        ImGui::SetTooltip("%s", tooltip.c_str());
    }

    if (playing)
    {
        const double   stepDelta = 1.0 / 60.0;
        CADacomb::GUID mergeGuid = currentMergeGuid;
        defer.defer(
          [this, &gatedDatabase, stepDelta, mergeGuid]()
          {
              if (!playing)
              {
                  return;
              }
              Polyglot::Node command = {
                  {      "type",           "StepTime" },
                  {   "version",                    1 },
                  { "deltaTime",            stepDelta },
                  { "mergeGuid", mergeGuid.toString() }
              };
              gatedDatabase.execute(command);
          });
    }
}

void TimeControls::toggle()
{
    playing = !playing;
    if (playing)
    {
        currentMergeGuid = CADacomb::GUID::random();
    }
}

void TimeControls::stop()
{
    playing = false;
}

bool TimeControls::isPlaying() const
{
    return playing;
}

void TimeControls::renderIcon() const
{
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2      minPos   = ImGui::GetItemRectMin();
    ImVec2      maxPos   = ImGui::GetItemRectMax();
    float       width    = maxPos.x - minPos.x;
    float       height   = maxPos.y - minPos.y;
    ImVec2      center   = ImVec2(minPos.x + width * 0.5f, minPos.y + height * 0.5f);
    ImU32       iconCol  = ImGui::GetColorU32(ImGuiCol_Text);

    if (!playing)
    {
        float  halfH = height * 0.24f;
        float  halfW = width * 0.20f;
        ImVec2 p1    = ImVec2(center.x - halfW * 0.7f, center.y - halfH);
        ImVec2 p2    = ImVec2(center.x - halfW * 0.7f, center.y + halfH);
        ImVec2 p3    = ImVec2(center.x + halfW * 1.15f, center.y);
        drawList->AddTriangleFilled(p1, p2, p3, iconCol);
    }
    else
    {
        float barW  = width * 0.12f;
        float gap   = width * 0.12f;
        float halfH = height * 0.24f;
        drawList->AddRectFilled(ImVec2(center.x - gap * 0.5f - barW, center.y - halfH), ImVec2(center.x - gap * 0.5f, center.y + halfH), iconCol);
        drawList->AddRectFilled(ImVec2(center.x + gap * 0.5f, center.y - halfH), ImVec2(center.x + gap * 0.5f + barW, center.y + halfH), iconCol);
    }
}

double TimeControls::queryCurrentTime(GatedDatabase& gatedDatabase) const
{
    Polyglot::Node response = gatedDatabase.execute({
      {    "type", "GetCurrentTime" },
      { "version",                1 }
    });
    return response["currentTime"].asDouble();
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("TimeControls/DefaultAndToggle")
{
    TimeControls controls;
    CHECK_FALSE(controls.isPlaying());

    controls.toggle();
    CHECK(controls.isPlaying());

    controls.toggle();
    CHECK_FALSE(controls.isPlaying());

    controls.toggle();
    CHECK(controls.isPlaying());
    controls.stop();
    CHECK_FALSE(controls.isPlaying());
}
#endif
