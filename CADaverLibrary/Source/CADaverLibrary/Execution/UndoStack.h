#pragma once

#include "Execution/UndoContainer.h"

#include <cstddef>
#include <memory>
#include <vector>

class GatedDatabase;

// Manages undo and redo command history stacks with step and memory capacity limits.
class UndoStack
{
  public:
    UndoStack(GatedDatabase& gatedDatabase);
    virtual ~UndoStack();

    void push(std::unique_ptr<UndoContainer> container);

    bool canUndo() const;
    bool canRedo() const;

    void undo();
    void redo();

    void clear();

    size_t getMaxSteps() const;
    void   setMaxSteps(size_t steps);

    size_t getMaxMemoryBytes() const;
    void   setMaxMemoryBytes(size_t bytes);

    size_t getTotalMemoryBytes() const;

    const std::vector<std::unique_ptr<UndoContainer>>& getUndoHistory() const;
    const std::vector<std::unique_ptr<UndoContainer>>& getRedoHistory() const;

  private:
    void prune();

    GatedDatabase&                              gatedDatabase;
    std::vector<std::unique_ptr<UndoContainer>> undoHistory;
    std::vector<std::unique_ptr<UndoContainer>> redoHistory;
    size_t                                      maxSteps       = 32;
    size_t                                      maxMemoryBytes = 1024ULL * 1024ULL * 1024ULL;
};
