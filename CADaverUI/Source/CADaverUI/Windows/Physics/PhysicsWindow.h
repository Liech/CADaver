#pragma once

#include "CADacomb/Util/GUID.h"
#include "Windows/Window.h"

#include <string>

// Window providing Play and Stop controls to advance the project physics simulation via mergeable commands.
class PhysicsWindow : public Window
{
  public:
    PhysicsWindow();
    virtual ~PhysicsWindow();

    std::string getType() const override;
    std::string getDescription() const override;
    void        render() override;

    bool isPlaying() const;

  private:
    CADacomb::GUID currentMergeGuid;
    double         elapsedTime = 0.0;
    bool           playing     = false;
};
