#pragma once

#include "CADacomb/Util/GUID.h"

class Defer;
class GatedDatabase;

// Renders a compact Play/Pause toggle button for stepping project time via mergeable StepTime commands.
class TimeControls
{
  public:
    TimeControls();
    virtual ~TimeControls();

    void render(GatedDatabase& gatedDatabase, Defer& defer);
    void toggle();
    void stop();

    bool isPlaying() const;

  private:
    void   renderIcon() const;
    double queryCurrentTime(GatedDatabase& gatedDatabase) const;

    bool           playing = false;
    CADacomb::GUID currentMergeGuid;
};
