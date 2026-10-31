#pragma once

#include "Database/Preferences/SVOPreferences.h"
#include "Execution/Command.h"

// Command to update SVO LOD visualization preferences.
class UpdateSVOPreferencesCommand : public Command
{
  public:
    UpdateSVOPreferencesCommand();
    virtual ~UpdateSVOPreferencesCommand();

    virtual std::string getType() const override;
    virtual int         getVersion() const override;
    virtual std::string getDescription() const override;

    virtual void           setInput(const nlohmann::json& json) override;
    virtual nlohmann::json execute(Database& database, std::istream* inputStream = nullptr, std::ostream* outputStream = nullptr) override;

    virtual bool   isUndoable() const override;
    virtual void   undo(Database& database) override;
    virtual void   redo(Database& database) override;
    virtual size_t getUndoSize() const override;

  private:
    nlohmann::json preferencesData;
    SVOPreferences previousPreferences;
    SVOPreferences newPreferences;
};
