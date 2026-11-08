#include "Execution/Defer.h"

Defer::Defer() {}

Defer::~Defer() {}

void Defer::defer(std::function<void()> task)
{
    std::lock_guard<std::mutex> lock(mutex);
    tasks.push_back(std::move(task));
}

void Defer::process()
{
    std::vector<std::function<void()>> toExecute;
    {
        std::lock_guard<std::mutex> lock(mutex);
        toExecute.swap(tasks);
    }

    for (const auto& task : toExecute)
    {
        if (task)
        {
            task();
        }
    }
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Defer/DeferAndProcess")
{
    Defer deferQueue;
    int   counter = 0;

    deferQueue.defer([&counter]() { counter += 2; });
    deferQueue.defer([&counter]() { counter += 3; });
    CHECK(counter == 0);

    deferQueue.process();
    CHECK(counter == 5);

    deferQueue.process();
    CHECK(counter == 5);
}
#endif
