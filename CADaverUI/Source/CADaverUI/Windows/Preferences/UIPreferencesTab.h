#pragma once

#include "Windows/Preferences/PreferenceTab.h"

class UIPreferences;

// Tab for configuring user interface scale, color theme, and viewport background color.
class UIPreferencesTab : public PreferenceTab
{
  public:
    UIPreferencesTab();
    virtual ~UIPreferencesTab();

    std::string getTitle() const override;
    bool        render(GatedDatabase& database, Defer& defer) override;

    static void apply(const UIPreferences& preferences);
};
