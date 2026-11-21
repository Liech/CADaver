#pragma once

#include "CADacomb/Util/GUID.h"

#include <cstddef>
#include <memory>
#include <string>

class Command;
class Database;

// Encapsulates an executed command for undo and redo operations without exposing the command instance.
class UndoContainer
{
  public:
    UndoContainer(std::unique_ptr<Command> command);
    virtual ~UndoContainer();

    std::string           getType() const;
    size_t                getUndoSize() const;
    bool                  isMergeable() const;
    const CADacomb::GUID& getMergeGuid() const;

    bool merge(const UndoContainer& other);
    void undo(Database& database);
    void redo(Database& database);

  private:
    std::unique_ptr<Command> command;
};
