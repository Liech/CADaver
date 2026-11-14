#pragma once

#include "CADacomb/Util/GUID.h"
#include "Execution/Command.h"

#include <string>

// Command to update the name and text of an existing text note component.
class UpdateNoteCommand : public Command
{
  public:
    UpdateNoteCommand();
    virtual ~UpdateNoteCommand();

    virtual std::string getType() const override;
    virtual int         getVersion() const override;
    virtual std::string getDescription() const override;

    virtual void           setInput(const Polyglot::Node& node) override;
    virtual nlohmann::json execute(Database& database) override;

    virtual bool   isUndoable() const override;
    virtual void   undo(Database& database) override;
    virtual void   redo(Database& database) override;
    virtual size_t getUndoSize() const override;

  private:
    CADacomb::GUID assetGuid;
    CADacomb::GUID componentGuid;
    std::string    newName;
    std::string    newText;
    std::string    previousName;
    std::string    previousText;
};
