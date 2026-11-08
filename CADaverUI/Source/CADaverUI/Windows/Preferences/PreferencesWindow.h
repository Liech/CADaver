#pragma once

#include "Windows/Preferences/PreferenceTab.h"
#include "Windows/Preferences/PreferenceTabFactory.h"
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

    void               selectTab(const std::string& tabName);
    const std::string& getSelectedTab() const;

    PreferenceTabFactory&       getTabFactory();
    const PreferenceTabFactory& getTabFactory() const;

    void load(const std::string& filepath = "preferences.json");
    void load(GatedDatabase& database, Defer& deferTarget, const std::string& filepath = "preferences.json");
    void save(const Preferences& preferences, const std::string& filepath = "preferences.json") const;

  private:
    PreferenceTabFactory                        tabFactory;
    std::vector<std::unique_ptr<PreferenceTab>> tabs;
    std::string                                 selectedTab;
};
