#pragma once

#include <glm/glm.hpp>
#include <nlohmann/json.hpp>
#include <string>

// Headless data model managing camera controls, sensitivity, and frustum clipping preferences.
class CameraPreferences
{
  public:
    CameraPreferences();
    virtual ~CameraPreferences();

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

    const glm::vec2& getResolution() const;
    void             setResolution(const glm::vec2& res);

    bool isResetRequested() const;
    void requestReset();
    void clearResetRequested();

    bool hasModeChangeRequest() const;
    int  getRequestedMode() const;
    void requestModeChange(int mode);
    void clearModeChangeRequest();

  private:
    glm::vec2 resolution          = glm::vec2(1280.0f, 720.0f);
    double    rotationSensitivity = 0.005;
    float     cameraSpeed         = 5.0f;
    float     fov                 = 1.3089969f;
    float     nearPlane           = 0.05f;
    float     farPlane            = 2000.0f;
    int       cameraMode          = 0;
    int       requestedMode       = 0;
    bool      resetRequested      = false;
    bool      modeChangeRequested = false;
};
