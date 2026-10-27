#include "Commands/Undo/PreferencesUndoRecord.h"

#include "Database/Database.h"

PreferencesUndoRecord::PreferencesUndoRecord(const std::string& categoryInput, nlohmann::json previousJsonInput, nlohmann::json newJsonInput)
  : category(categoryInput)
  , previousJson(std::move(previousJsonInput))
  , newJson(std::move(newJsonInput))
{
}

PreferencesUndoRecord::~PreferencesUndoRecord() {}

void PreferencesUndoRecord::undo(Database& database)
{
    applyJson(database, previousJson);
}

void PreferencesUndoRecord::redo(Database& database)
{
    applyJson(database, newJson);
}

std::string PreferencesUndoRecord::getDescription() const
{
    return "Update " + category + " Preferences";
}

size_t PreferencesUndoRecord::getSizeInBytes() const
{
    size_t size = sizeof(PreferencesUndoRecord);
    size += category.capacity();
    size += previousJson.dump().capacity();
    size += newJson.dump().capacity();
    return size;
}

const std::string& PreferencesUndoRecord::getCategory() const
{
    return category;
}

const nlohmann::json& PreferencesUndoRecord::getPreviousJson() const
{
    return previousJson;
}

const nlohmann::json& PreferencesUndoRecord::getNewJson() const
{
    return newJson;
}

void PreferencesUndoRecord::applyJson(Database& database, const nlohmann::json& json)
{
    auto& prefs = database.getPreferences();
    if (category == "UI")
    {
        prefs.getUIPreferences().fromJson(json);
    }
    else if (category == "Camera")
    {
        prefs.getCameraPreferences().fromJson(json);
    }
    else if (category == "Render")
    {
        prefs.getRenderPreferences().fromJson(json);
    }
    else if (category == "SVO")
    {
        prefs.getSVOPreferences().fromJson(json);
    }
    prefs.save();
}
