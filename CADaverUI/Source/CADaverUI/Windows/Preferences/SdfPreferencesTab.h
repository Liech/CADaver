#pragma once

#include "Windows/Preferences/PreferenceTab.h"

// Preference tab for configuring SDF sphere-tracing and shading parameters.
class SdfPreferencesTab : public PreferenceTab
{
  public:
    SdfPreferencesTab();
    virtual ~SdfPreferencesTab();

    virtual std::string getTitle() const override;
    virtual std::string getDescription() const override;
    virtual bool        render(GatedDatabase& gatedDatabase, Defer& defer) override;
};
