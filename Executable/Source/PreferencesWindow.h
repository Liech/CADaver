#pragma once

#include <memory>

class PreferencesData;
class UIPreferences;

class PreferencesWindow
{
  public:
    PreferencesWindow();
    virtual ~PreferencesWindow();

    void render(PreferencesData& preferences);

    bool isOpen() const;
    void setOpen(bool open);

  private:
    std::unique_ptr<UIPreferences> uiPreferences;
    bool                           isOpenState = false;
};
