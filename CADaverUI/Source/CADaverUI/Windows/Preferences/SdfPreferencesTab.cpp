#include "Windows/Preferences/SdfPreferencesTab.h"

#include "Database/Preferences/SdfPreferences.h"
#include "Execution/Defer.h"
#include "Execution/GatedDatabase.h"

#include <imgui.h>

SdfPreferencesTab::SdfPreferencesTab() {}

SdfPreferencesTab::~SdfPreferencesTab() {}

std::string SdfPreferencesTab::getTitle() const
{
    return "SDF";
}

std::string SdfPreferencesTab::getDescription() const
{
    return "Preference Tab: SDF\n"
           "Purpose: Configures GPU sphere-tracing and shading parameters (max steps, hit epsilon, step scale, normal epsilon, ambient strength) for SdfAsset rendering.";
}

bool SdfPreferencesTab::render(GatedDatabase& gatedDatabase, Defer& defer)
{
    Polyglot::Node sdfRes = gatedDatabase.execute({
      {    "type", "GetSdfPreferences" },
      { "version",                   1 }
    });
    SdfPreferences sdfPrefs;
    sdfPrefs.fromNode(sdfRes["preferences"]);

    bool changed = false;

    int maxSteps = sdfPrefs.getMaxSteps();
    if (ImGui::SliderInt("Max Steps", &maxSteps, 16, 1024))
    {
        sdfPrefs.setMaxSteps(maxSteps);
        changed = true;
    }

    float hitEpsilon = static_cast<float>(sdfPrefs.getHitEpsilon());
    if (ImGui::DragFloat("Hit Epsilon", &hitEpsilon, 0.00005f, 0.00001f, 0.05f, "%.5f"))
    {
        sdfPrefs.setHitEpsilon(static_cast<double>(hitEpsilon));
        changed = true;
    }

    float stepScale = static_cast<float>(sdfPrefs.getStepScale());
    if (ImGui::SliderFloat("Step Scale", &stepScale, 0.1f, 1.0f, "%.2f"))
    {
        sdfPrefs.setStepScale(static_cast<double>(stepScale));
        changed = true;
    }

    float normalEpsilon = static_cast<float>(sdfPrefs.getNormalEpsilon());
    if (ImGui::DragFloat("Normal Epsilon", &normalEpsilon, 0.0001f, 0.00005f, 0.05f, "%.5f"))
    {
        sdfPrefs.setNormalEpsilon(static_cast<double>(normalEpsilon));
        changed = true;
    }

    float ambientStrength = static_cast<float>(sdfPrefs.getAmbientStrength());
    if (ImGui::SliderFloat("Ambient Strength", &ambientStrength, 0.0f, 1.0f, "%.2f"))
    {
        sdfPrefs.setAmbientStrength(static_cast<double>(ambientStrength));
        changed = true;
    }

    if (changed)
    {
        Polyglot::Node sdfNode = sdfPrefs.toNode();
        defer.defer(
          [&gatedDatabase, sdfNode]()
          {
              Polyglot::Node command = {
                  {        "type", "UpdateSdfPreferences" },
                  {     "version",                      1 },
                  { "preferences",                sdfNode }
              };
              gatedDatabase.execute(command);
          });
    }

    return changed;
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("SdfPreferencesTab/Title")
{
    SdfPreferencesTab tab;
    CHECK(tab.getTitle() == "SDF");
}
#endif
