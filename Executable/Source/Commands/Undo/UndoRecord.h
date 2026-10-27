#pragma once

#include <cstddef>
#include <string>

class Database;

class UndoRecord
{
  public:
    UndoRecord();
    virtual ~UndoRecord();

    virtual void undo(Database& database) = 0;
    virtual void redo(Database& database) = 0;

    virtual std::string getDescription() const = 0;
    virtual size_t      getSizeInBytes() const = 0;
};
