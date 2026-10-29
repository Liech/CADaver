#pragma once

#include "CADacomb/Util/GUID.h"

#include <atomic>
#include <mutex>
#include <string>

// Modal progress bar popup rendering progress updates and supporting cancellation.
class ModalProgressBar
{
  public:
    ModalProgressBar();
    virtual ~ModalProgressBar();

    void        setProgress(float progress, const std::string& message);
    float       getProgress() const;
    std::string getMessage() const;

    void                  setGuid(const CADacomb::GUID& guid);
    const CADacomb::GUID& getGuid() const;

    void render(const std::string& title);
    void close();
    void reset();

  private:
    std::atomic<float> progress = 0.0f;
    mutable std::mutex messageMutex;
    std::string        message = "";
    CADacomb::GUID     guid;
    bool               shouldClose = false;
};
