#pragma once

#include <string>

class Defer;
class GatedDatabase;

// Abstract base class representing a single tab within PreferencesWindow.
class PreferenceTab
{
  public:
    PreferenceTab();
    virtual ~PreferenceTab();

    virtual std::string getTitle() const                              = 0;
    virtual bool        render(GatedDatabase& database, Defer& defer) = 0;
};
