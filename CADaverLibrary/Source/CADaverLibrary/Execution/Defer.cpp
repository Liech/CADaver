#include "Execution/Defer.h"

Defer::Defer() {}

Defer::~Defer() {}

Defer& Defer::getInstance()
{
    static Defer instance;
    return instance;
}

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
