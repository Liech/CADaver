#pragma once

#include <functional>
#include <mutex>
#include <vector>

// Singleton queue for deferring lambda execution to the onLogic frame phase.
class Defer
{
  public:
    Defer();
    virtual ~Defer();

    static Defer& getInstance();

    void defer(std::function<void()> task);
    void process();

  private:
    std::vector<std::function<void()>> tasks;
    std::mutex                         mutex;
};
