#pragma once

#include "Database/Preferences/UIPreferences.h"
#include "Execution/Command.h"

// Command to update UI preferences (theme, UI scale, background color).
class UpdateUIPreferencesCommand : public Command
{
  public:
    UpdateUIPreferencesCommand();
    explicit UpdateUIPreferencesCommand(const nlohmann::json& json);
    virtual ~UpdateUIPreferencesCommand();

    virtual std::string getType() const override;
    virtual int         getVersion() const override;
    virtual std::string getDescription() const override;

    virtual void           setInput(const nlohmann::json& json) override;
    virtual nlohmann::json execute(Database& database, std::istream* inputStream = nullptr, std::ostream* outputStream = nullptr) override;

    virtual bool   isUndoable() const override;
    virtual void   undo(Database& database) override;
    virtual void   redo(Database& database) override;
    virtual size_t getUndoSize() const override;

    const nlohmann::json& getPreferencesData() const;
    void                  setPreferencesData(const nlohmann::json& data);

  private:
    nlohmann::json preferencesData;
    UIPreferences  previousPreferences;
    UIPreferences  newPreferences;
};
