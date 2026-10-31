#pragma once

#include "CADacomb/Util/GUID.h"
#include "Execution/Command.h"

#include <memory>
#include <string>

class TextNoteComponent;

// Command to add a text note component to a specified asset.
class AddNoteCommand : public Command
{
  public:
    AddNoteCommand();
    virtual ~AddNoteCommand();

    std::string getType() const override;
    int         getVersion() const override;
    std::string getDescription() const override;

    void           setInput(const nlohmann::json& json) override;
    nlohmann::json execute(Database& database, std::istream* inputStream = nullptr, std::ostream* outputStream = nullptr) override;

    bool   isUndoable() const override;
    void   undo(Database& database) override;
    void   redo(Database& database) override;
    size_t getUndoSize() const override;

  private:
    CADacomb::GUID                     assetGuid;
    std::string                        name = "Note";
    std::string                        text = "";
    std::shared_ptr<TextNoteComponent> addedComponent;
};
