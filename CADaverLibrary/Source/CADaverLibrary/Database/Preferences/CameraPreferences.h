#pragma once

#include "Polyglot/Node.h"

#include <nlohmann/json.hpp>
#include <string>

// Headless data model managing camera controls, sensitivity, and frustum clipping preferences.
class CameraPreferences
{
  public:
    CameraPreferences();
    virtual ~CameraPreferences();

    Polyglot::Node toNode() const;
    void           fromNode(const Polyglot::Node& node);

    nlohmann::json toJson() const;
    void           fromJson(const nlohmann::json& jsonObject);

    int  getCameraMode() const;
    void setCameraMode(int mode);

    float getCameraSpeed() const;
    void  setCameraSpeed(float speed);

    double getRotationSensitivity() const;
    void   setRotationSensitivity(double sensitivity);

    float getFov() const;
    void  setFov(float fovVal);

    float getNearPlane() const;
    void  setNearPlane(float nearVal);

    float getFarPlane() const;
    void  setFarPlane(float farVal);

    bool isOrthographic() const;
    void setOrthographic(bool orthographic);

    bool hasModeChangeRequest() const;
    int  getRequestedMode() const;
    void requestModeChange(int mode);
    void clearModeChangeRequest();

  private:
    double rotationSensitivity = 0.005;
    float  cameraSpeed         = 5.0f;
    float  fov                 = 1.3089969f;
    float  nearPlane           = 0.05f;
    float  farPlane            = 2000.0f;
    int    cameraMode          = 0;
    int    requestedMode       = 0;
    bool   orthographic        = false;
    bool   modeChangeRequested = false;
};
