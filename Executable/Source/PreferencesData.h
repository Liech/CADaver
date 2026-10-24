#pragma once

#include <string>

class PreferencesData
{
  public:
    PreferencesData();
    virtual ~PreferencesData();

    bool load();
    bool save() const;
    void apply() const;

    float getUiScale() const;
    void  setUiScale(float scale);

    int  getThemeIndex() const;
    void setThemeIndex(int index);

    const std::string& getFilePath() const;
    void               setFilePath(const std::string& path);

  private:
    float       uiScale        = 1.0f;
    int         themeIndex     = 0;
    std::string configFilePath = "preferences.json";
};
