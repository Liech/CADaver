#include "Commands/Undo/UndoStack.h"

#include "Database/Database.h"

UndoStack::UndoStack() {}

UndoStack::~UndoStack() {}

void UndoStack::push(std::unique_ptr<UndoRecord> record)
{
    if (!record)
    {
        return;
    }
    redoHistory.clear();
    undoHistory.push_back(std::move(record));
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

void UndoStack::undo(Database& database)
{
    if (undoHistory.empty())
    {
        return;
    }
    auto record = std::move(undoHistory.back());
    undoHistory.pop_back();
    record->undo(database);
    redoHistory.push_back(std::move(record));
}

void UndoStack::redo(Database& database)
{
    if (redoHistory.empty())
    {
        return;
    }
    auto record = std::move(redoHistory.back());
    redoHistory.pop_back();
    record->redo(database);
    undoHistory.push_back(std::move(record));
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
    for (const auto& record : undoHistory)
    {
        if (record)
        {
            total += record->getSizeInBytes();
        }
    }
    for (const auto& record : redoHistory)
    {
        if (record)
        {
            total += record->getSizeInBytes();
        }
    }
    return total;
}

const std::vector<std::unique_ptr<UndoRecord>>& UndoStack::getUndoHistory() const
{
    return undoHistory;
}

const std::vector<std::unique_ptr<UndoRecord>>& UndoStack::getRedoHistory() const
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
#include "Commands/Undo/AddAssetUndoRecord.h"
#include "Database/Asset/TriangulationAsset.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("UndoStack/PushUndoRedo")
{
    Database  database;
    UndoStack stack;

    REQUIRE_FALSE(stack.canUndo());
    REQUIRE_FALSE(stack.canRedo());

    auto asset = std::make_shared<TriangulationAsset>();
    asset->setName("Test");
    database.getProject().addAsset(asset);
    stack.push(std::make_unique<AddAssetUndoRecord>(asset));

    REQUIRE(stack.canUndo());
    REQUIRE_FALSE(stack.canRedo());
    REQUIRE(database.getProject().getAssetCount() == 1);

    stack.undo(database);
    REQUIRE_FALSE(stack.canUndo());
    REQUIRE(stack.canRedo());
    REQUIRE(database.getProject().getAssetCount() == 0);

    stack.redo(database);
    REQUIRE(stack.canUndo());
    REQUIRE_FALSE(stack.canRedo());
    REQUIRE(database.getProject().getAssetCount() == 1);
}

TEST_CASE("UndoStack/PruneMaxSteps")
{
    Database  database;
    UndoStack stack;
    stack.setMaxSteps(3);

    for (size_t i = 0; i < 5; i++)
    {
        auto asset = std::make_shared<TriangulationAsset>();
        asset->setName("Asset" + std::to_string(i));
        stack.push(std::make_unique<AddAssetUndoRecord>(asset));
    }

    REQUIRE(stack.getUndoHistory().size() == 3);
}

TEST_CASE("UndoStack/MemoryCapacityTracking")
{
    UndoStack stack;
    auto      asset = std::make_shared<TriangulationAsset>();
    asset->setName("Asset");
    stack.push(std::make_unique<AddAssetUndoRecord>(asset));

    REQUIRE(stack.getTotalMemoryBytes() > 0);
}
#endif
