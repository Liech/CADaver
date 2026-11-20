#pragma once

#include "UI/Gizmo/GizmoMode.h"

// Small viewport overlay window for choosing gizmo mode, space and pivot. Owns those settings.
class GizmoOverlay
{
  public:
    GizmoOverlay();
    virtual ~GizmoOverlay();

    bool render();

    GizmoMode getMode() const;
    void      setMode(GizmoMode mode);

    bool isRotatingAroundCenter() const;
    bool isLocalSpace() const;

  private:
    GizmoMode mode               = GizmoMode::Translate;
    bool      rotateAroundCenter = false;
    bool      localSpace         = false;
};
