#pragma once

#include "Windows/Preferences/PreferenceTab.h"

#include <functional>
#include <memory>
#include <vector>

// Singleton factory registering and creating preference tabs for PreferencesWindow.
class PreferenceTabFactory
{
  public:
    static PreferenceTabFactory& getInstance();

    PreferenceTabFactory();
    virtual ~PreferenceTabFactory();

    void registerTab(std::function<std::unique_ptr<PreferenceTab>()> creator);
    void registerDefaultTabs();
    void clear();

    std::vector<std::unique_ptr<PreferenceTab>> createTabs() const;

  private:
    std::vector<std::function<std::unique_ptr<PreferenceTab>()>> tabCreators;
};
