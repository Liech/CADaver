#pragma once

#include "Windows/Preferences/PreferenceTab.h"

// Tab for configuring mesh display colors, wireframe overlay, and backface culling options.
class RenderPreferencesTab : public PreferenceTab
{
  public:
    RenderPreferencesTab();
    virtual ~RenderPreferencesTab();

    std::string getTitle() const override;
    bool        render(GatedDatabase& database, Defer& defer) override;
};
