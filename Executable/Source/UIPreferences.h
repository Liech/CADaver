#pragma once

#include <string>

class PreferencesData;

class UIPreferences
{
  public:
    UIPreferences();
    virtual ~UIPreferences();

    const std::string& getTitle() const;
    bool               render(PreferencesData& preferences);

  private:
    std::string title = "UI";
};
