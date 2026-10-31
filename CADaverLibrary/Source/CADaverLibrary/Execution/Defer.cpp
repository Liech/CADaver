#include "Execution/Defer.h"

#include "Execution/GatedDatabase.h"

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

    if (toExecute.empty())
    {
        return;
    }

    GatedDatabase::getInstance().unlock();
    for (const auto& task : toExecute)
    {
        if (task)
        {
            task();
        }
    }
    GatedDatabase::getInstance().lock();
}
