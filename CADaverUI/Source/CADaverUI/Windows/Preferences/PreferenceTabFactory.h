#pragma once

#include "Windows/Preferences/PreferenceTab.h"

#include <functional>
#include <memory>
#include <string>
#include <vector>

// Factory registering and creating preference tabs for PreferencesWindow.
class PreferenceTabFactory
{
  public:
    PreferenceTabFactory();
    virtual ~PreferenceTabFactory();

    void registerTab(const std::string& name, std::function<std::unique_ptr<PreferenceTab>()> creator);
    void registerDefaultTabs();
    void clear();

    std::vector<std::string>                    getRegisteredTabs() const;
    std::vector<std::unique_ptr<PreferenceTab>> createTabs() const;
    std::unique_ptr<PreferenceTab>              createTab(const std::string& name) const;

  private:
    std::vector<std::string>                                     tabNames;
    std::vector<std::function<std::unique_ptr<PreferenceTab>()>> tabCreators;
};
