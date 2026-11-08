#pragma once

#include "Windows/Preferences/PreferenceTab.h"

// Tab for configuring SVO chunk sizing, vertex capacity, rebuild requests, and LOD metrics.
class SVOPreferencesTab : public PreferenceTab
{
  public:
    SVOPreferencesTab();
    virtual ~SVOPreferencesTab();

    std::string getTitle() const override;
    std::string getDescription() const override;
    bool        render(GatedDatabase& database, Defer& defer) override;
};
