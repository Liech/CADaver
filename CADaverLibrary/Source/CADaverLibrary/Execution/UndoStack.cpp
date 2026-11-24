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
    if (!undoHistory.empty())
    {
        auto& top = undoHistory.back();
        if (top && top->isMergeable() && container->isMergeable() && !isZeroGuid(top->getMergeGuid()) && top->getMergeGuid() == container->getMergeGuid())
        {
            if (top->merge(*container))
            {
                prune();
                return;
            }
        }
    }
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
    if (onUndoCallback)
    {
        onUndoCallback();
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

void UndoStack::setOnUndoCallback(std::function<void()> callback)
{
    onUndoCallback = std::move(callback);
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

bool UndoStack::isZeroGuid(const CADacomb::GUID& guid) const
{
    for (char c : guid.toString())
    {
        if (c != '0' && c != '-')
        {
            return false;
        }
    }
    return true;
}

#ifdef ISTESTPROJECT
#include "Database/Asset/TriangulationAsset.h"
#include "Utility/MatrixJson.h"

#include <catch2/catch_test_macros.hpp>
#include <glm/gtc/matrix_transform.hpp>

TEST_CASE("UndoStack/InitialState")
{
    GatedDatabase gatedDatabase;
    UndoStack     stack(gatedDatabase);

    REQUIRE_FALSE(stack.canUndo());
    REQUIRE_FALSE(stack.canRedo());
    REQUIRE(stack.getTotalMemoryBytes() == 0);
}

TEST_CASE("UndoStack/MergesTransformCommandsWithSameMergeGuid")
{
    GatedDatabase gatedDatabase;
    UndoStack     stack(gatedDatabase);

    Polyglot::Node genRes    = gatedDatabase.execute({
      {    "type", "GenerateBaseShape" },
      { "version",                   1 },
      {   "shape",              "cube" }
    });
    std::string    assetGuid = genRes["assetGuid"].asString();
    stack.clear();

    CADacomb::GUID mergeGuid = CADacomb::GUID::random();
    glm::dmat4     t1        = glm::translate(glm::dmat4(1.0), glm::dvec3(1.0, 0.0, 0.0));
    glm::dmat4     t2        = glm::translate(glm::dmat4(1.0), glm::dvec3(5.0, 0.0, 0.0));

    gatedDatabase.execute({
      {           "type",       "TransformAsset" },
      {        "version",                      1 },
      {      "assetGuid",              assetGuid },
      { "transformation", MatrixJson::toJson(t1) },
      {      "mergeGuid",   mergeGuid.toString() }
    });
    gatedDatabase.execute({
      {           "type",       "TransformAsset" },
      {        "version",                      1 },
      {      "assetGuid",              assetGuid },
      { "transformation", MatrixJson::toJson(t2) },
      {      "mergeGuid",   mergeGuid.toString() }
    });

    REQUIRE(stack.getUndoHistory().size() == 1);

    stack.undo();
    Polyglot::Node afterUndo = gatedDatabase.execute({
      {      "type", "GetTransformationAsset" },
      {   "version",                        1 },
      { "assetGuid",                assetGuid }
    });
    CHECK(MatrixJson::fromJson(afterUndo["transformation"].toJson()) == glm::dmat4(1.0));

    stack.redo();
    Polyglot::Node afterRedo = gatedDatabase.execute({
      {      "type", "GetTransformationAsset" },
      {   "version",                        1 },
      { "assetGuid",                assetGuid }
    });
    CHECK(MatrixJson::fromJson(afterRedo["transformation"].toJson()) == t2);
}

TEST_CASE("UndoStack/DoesNotMergeWithZeroOrDifferentMergeGuid")
{
    GatedDatabase gatedDatabase;
    UndoStack     stack(gatedDatabase);

    Polyglot::Node genRes    = gatedDatabase.execute({
      {    "type", "GenerateBaseShape" },
      { "version",                   1 },
      {   "shape",              "cube" }
    });
    std::string    assetGuid = genRes["assetGuid"].asString();
    stack.clear();

    glm::dmat4 t1 = glm::translate(glm::dmat4(1.0), glm::dvec3(1.0, 0.0, 0.0));
    glm::dmat4 t2 = glm::translate(glm::dmat4(1.0), glm::dvec3(2.0, 0.0, 0.0));
    glm::dmat4 t3 = glm::translate(glm::dmat4(1.0), glm::dvec3(3.0, 0.0, 0.0));

    gatedDatabase.execute({
      {           "type",                       "TransformAsset" },
      {        "version",                                      1 },
      {      "assetGuid",                              assetGuid },
      { "transformation",                 MatrixJson::toJson(t1) },
      {      "mergeGuid", "00000000-0000-0000-0000-000000000000" }
    });
    gatedDatabase.execute({
      {           "type",                       "TransformAsset" },
      {        "version",                                      1 },
      {      "assetGuid",                              assetGuid },
      { "transformation",                 MatrixJson::toJson(t2) },
      {      "mergeGuid", "00000000-0000-0000-0000-000000000000" }
    });
    gatedDatabase.execute({
      {           "type",                    "TransformAsset" },
      {        "version",                                   1 },
      {      "assetGuid",                           assetGuid },
      { "transformation",              MatrixJson::toJson(t3) },
      {      "mergeGuid", CADacomb::GUID::random().toString() }
    });

    CHECK(stack.getUndoHistory().size() == 3);
}

TEST_CASE("UndoStack/OnUndoCallback")
{
    GatedDatabase gatedDatabase;
    UndoStack     stack(gatedDatabase);

    int callbackCount = 0;
    stack.setOnUndoCallback([&callbackCount]() { callbackCount++; });

    stack.undo();
    CHECK(callbackCount == 0);

    gatedDatabase.execute({
      {    "type", "GenerateBaseShape" },
      { "version",                   1 },
      {   "shape",              "cube" }
    });
    stack.undo();
    CHECK(callbackCount == 1);
}
#endif
