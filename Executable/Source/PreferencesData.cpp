#include "PreferencesData.h"

#include <filesystem>
#include <fstream>
#include <imgui.h>
#include <nlohmann/json.hpp>

PreferencesData::PreferencesData() {}

PreferencesData::~PreferencesData() {}

bool PreferencesData::load()
{
    if (!std::filesystem::exists(configFilePath))
    {
        return false;
    }

    try
    {
        std::ifstream file(configFilePath);
        if (!file.is_open())
        {
            return false;
        }

        nlohmann::json jsonObject;
        file >> jsonObject;

        if (jsonObject.contains("uiScale") && jsonObject["uiScale"].is_number())
        {
            uiScale = jsonObject["uiScale"].get<float>();
        }

        if (jsonObject.contains("themeIndex") && jsonObject["themeIndex"].is_number_integer())
        {
            themeIndex = jsonObject["themeIndex"].get<int>();
        }

        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool PreferencesData::save() const
{
    try
    {
        nlohmann::json jsonObject;
        jsonObject["uiScale"]    = uiScale;
        jsonObject["themeIndex"] = themeIndex;

        std::ofstream file(configFilePath);
        if (!file.is_open())
        {
            return false;
        }

        file << jsonObject.dump(4);
        return true;
    }
    catch (...)
    {
        return false;
    }
}

void PreferencesData::apply() const
{
    ImGuiIO& io        = ImGui::GetIO();
    io.FontGlobalScale = uiScale;

    switch (themeIndex)
    {
        case 1:
            ImGui::StyleColorsLight();
            break;
        case 2:
            ImGui::StyleColorsClassic();
            break;
        default:
            ImGui::StyleColorsDark();
            break;
    }
}

float PreferencesData::getUiScale() const
{
    return uiScale;
}

void PreferencesData::setUiScale(float scale)
{
    uiScale = scale;
}

int PreferencesData::getThemeIndex() const
{
    return themeIndex;
}

void PreferencesData::setThemeIndex(int index)
{
    themeIndex = index;
}

const std::string& PreferencesData::getFilePath() const
{
    return configFilePath;
}

void PreferencesData::setFilePath(const std::string& path)
{
    configFilePath = path;
}
