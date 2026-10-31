#pragma once

#include "Windows/Preferences/PreferenceTab.h"
#include "Windows/Window.h"

#include <memory>
#include <string>
#include <vector>

class Preferences;

// Window managing preference tabs and saving and loading persistent preferences.
class PreferencesWindow : public Window
{
  public:
    PreferencesWindow();
    virtual ~PreferencesWindow();

    std::string getType() const override;
    std::string getDescription() const override;
    void        render() override;

    void load(const std::string& filepath = "preferences.json");
    void load(GatedDatabase& database, Defer& deferTarget, const std::string& filepath = "preferences.json");
    void save(const Preferences& preferences, const std::string& filepath = "preferences.json") const;

  private:
    std::vector<std::unique_ptr<PreferenceTab>> tabs;
};
