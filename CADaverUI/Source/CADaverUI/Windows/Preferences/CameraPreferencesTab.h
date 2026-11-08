#pragma once

#include "Windows/Preferences/PreferenceTab.h"

// Tab for configuring camera navigation mode, projection, sensitivity, and clipping planes.
class CameraPreferencesTab : public PreferenceTab
{
  public:
    CameraPreferencesTab();
    virtual ~CameraPreferencesTab();

    std::string getTitle() const override;
    std::string getDescription() const override;
    bool        render(GatedDatabase& database, Defer& defer) override;
};
