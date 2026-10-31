#include "Execution/UndoStack.h"

#include "Execution/GatedDatabase.h"

UndoStack::UndoStack(GatedDatabase& gatedDatabaseInput)
  : gatedDatabase(gatedDatabaseInput)
{
    gatedDatabase.setUndoCallback([this](std::unique_ptr<UndoContainer> container) { push(std::move(container)); });
}

UndoStack::~UndoStack() {}

void UndoStack::push(std::unique_ptr<UndoContainer> container)
{
    if (!container)
    {
        return;
    }
    redoHistory.clear();
    undoHistory.push_back(std::move(container));
    prune();
}

bool UndoStack::canUndo() const
{
    return !undoHistory.empty();
}

bool UndoStack::canRedo() const
{
    return !redoHistory.empty();
}

void UndoStack::undo()
{
    if (undoHistory.empty())
    {
        return;
    }
    auto container = std::move(undoHistory.back());
    undoHistory.pop_back();
    gatedDatabase.undo(*container);
    redoHistory.push_back(std::move(container));
}

void UndoStack::redo()
{
    if (redoHistory.empty())
    {
        return;
    }
    auto container = std::move(redoHistory.back());
    redoHistory.pop_back();
    gatedDatabase.redo(*container);
    undoHistory.push_back(std::move(container));
}

void UndoStack::clear()
{
    undoHistory.clear();
    redoHistory.clear();
}

size_t UndoStack::getMaxSteps() const
{
    return maxSteps;
}

void UndoStack::setMaxSteps(size_t steps)
{
    maxSteps = steps;
    prune();
}

size_t UndoStack::getMaxMemoryBytes() const
{
    return maxMemoryBytes;
}

void UndoStack::setMaxMemoryBytes(size_t bytes)
{
    maxMemoryBytes = bytes;
    prune();
}

size_t UndoStack::getTotalMemoryBytes() const
{
    size_t total = 0;
    for (const auto& container : undoHistory)
    {
        if (container)
        {
            total += container->getUndoSize();
        }
    }
    for (const auto& container : redoHistory)
    {
        if (container)
        {
            total += container->getUndoSize();
        }
    }
    return total;
}

const std::vector<std::unique_ptr<UndoContainer>>& UndoStack::getUndoHistory() const
{
    return undoHistory;
}

const std::vector<std::unique_ptr<UndoContainer>>& UndoStack::getRedoHistory() const
{
    return redoHistory;
}

void UndoStack::prune()
{
    while (undoHistory.size() > maxSteps)
    {
        undoHistory.erase(undoHistory.begin());
    }

    while (undoHistory.size() > 1 && getTotalMemoryBytes() > maxMemoryBytes)
    {
        undoHistory.erase(undoHistory.begin());
    }
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("UndoStack/InitialState")
{
    GatedDatabase gatedDatabase;
    UndoStack     stack(gatedDatabase);

    REQUIRE_FALSE(stack.canUndo());
    REQUIRE_FALSE(stack.canRedo());
    REQUIRE(stack.getTotalMemoryBytes() == 0);
}
#endif
