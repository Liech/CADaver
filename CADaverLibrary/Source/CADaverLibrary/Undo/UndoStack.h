#pragma once

#include "Undo/UndoRecord.h"

#include <cstddef>
#include <memory>
#include <vector>

class Database;

class UndoStack
{
  public:
    UndoStack();
    virtual ~UndoStack();

    static UndoStack& getInstance();

    void push(std::unique_ptr<UndoRecord> record);

    bool canUndo() const;
    bool canRedo() const;

    void undo(Database& database);
    void redo(Database& database);

    void clear();

    size_t getMaxSteps() const;
    void   setMaxSteps(size_t steps);

    size_t getMaxMemoryBytes() const;
    void   setMaxMemoryBytes(size_t bytes);

    size_t getTotalMemoryBytes() const;

    const std::vector<std::unique_ptr<UndoRecord>>& getUndoHistory() const;
    const std::vector<std::unique_ptr<UndoRecord>>& getRedoHistory() const;

  private:
    void prune();

    std::vector<std::unique_ptr<UndoRecord>> undoHistory;
    std::vector<std::unique_ptr<UndoRecord>> redoHistory;
    size_t                                   maxSteps       = 32;
    size_t                                   maxMemoryBytes = 1024ULL * 1024ULL * 1024ULL;
};
