#pragma once

#include "Database/Preferences/SdfPreferences.h"
#include "Execution/Command.h"
#include "Polyglot/Node.h"

// Updates SDF sphere-tracing and shading preferences with undo/redo support.
class UpdateSdfPreferencesCommand : public Command
{
  public:
    UpdateSdfPreferencesCommand();
    virtual ~UpdateSdfPreferencesCommand();

    virtual std::string    getType() const override;
    virtual int            getVersion() const override;
    virtual std::string    getDescription() const override;
    virtual void           setInput(const Polyglot::Node& node) override;
    virtual nlohmann::json execute(Database& database) override;

    virtual bool   isUndoable() const override;
    virtual void   undo(Database& database) override;
    virtual void   redo(Database& database) override;
    virtual size_t getUndoSize() const override;

  private:
    Polyglot::Node preferencesData;
    SdfPreferences previousPreferences;
    SdfPreferences newPreferences;
};
