#pragma once

#include "Undo/UndoRecord.h"

#include <nlohmann/json.hpp>
#include <string>

class PreferencesUndoRecord : public UndoRecord
{
  public:
    PreferencesUndoRecord(const std::string& category, nlohmann::json previousJson, nlohmann::json newJson);
    virtual ~PreferencesUndoRecord();

    void undo(Database& database) override;
    void redo(Database& database) override;

    std::string getDescription() const override;
    size_t      getSizeInBytes() const override;

    const std::string&    getCategory() const;
    const nlohmann::json& getPreviousJson() const;
    const nlohmann::json& getNewJson() const;

  private:
    void applyJson(Database& database, const nlohmann::json& json);

    std::string    category;
    nlohmann::json previousJson;
    nlohmann::json newJson;
};
