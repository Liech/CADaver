#include "Database/Preferences/CameraPreferences.h"

CameraPreferences::CameraPreferences() {}

CameraPreferences::~CameraPreferences() {}

nlohmann::json CameraPreferences::toJson() const
{
    nlohmann::json jsonObject;
    jsonObject["cameraMode"]          = cameraMode;
    jsonObject["cameraSpeed"]         = cameraSpeed;
    jsonObject["rotationSensitivity"] = rotationSensitivity;
    jsonObject["fov"]                 = fov;
    jsonObject["nearPlane"]           = nearPlane;
    jsonObject["farPlane"]            = farPlane;
    jsonObject["resolution"]          = { resolution.x, resolution.y };
    return jsonObject;
}

void CameraPreferences::fromJson(const nlohmann::json& jsonObject)
{
    cameraMode          = jsonObject.at("cameraMode").get<int>();
    cameraSpeed         = jsonObject.at("cameraSpeed").get<float>();
    rotationSensitivity = jsonObject.at("rotationSensitivity").get<double>();
    fov                 = jsonObject.at("fov").get<float>();
    nearPlane           = jsonObject.at("nearPlane").get<float>();
    farPlane            = jsonObject.at("farPlane").get<float>();

    const auto& resArray = jsonObject.at("resolution");
    resolution.x         = resArray.at(0).get<float>();
    resolution.y         = resArray.at(1).get<float>();
}

int CameraPreferences::getCameraMode() const
{
    return cameraMode;
}

void CameraPreferences::setCameraMode(int mode)
{
    cameraMode = mode;
}

float CameraPreferences::getCameraSpeed() const
{
    return cameraSpeed;
}

void CameraPreferences::setCameraSpeed(float speed)
{
    cameraSpeed = speed;
}

double CameraPreferences::getRotationSensitivity() const
{
    return rotationSensitivity;
}

void CameraPreferences::setRotationSensitivity(double sensitivity)
{
    rotationSensitivity = sensitivity;
}

float CameraPreferences::getFov() const
{
    return fov;
}

void CameraPreferences::setFov(float fovVal)
{
    fov = fovVal;
}

float CameraPreferences::getNearPlane() const
{
    return nearPlane;
}

void CameraPreferences::setNearPlane(float nearVal)
{
    nearPlane = nearVal;
}

float CameraPreferences::getFarPlane() const
{
    return farPlane;
}

void CameraPreferences::setFarPlane(float farVal)
{
    farPlane = farVal;
}

const glm::vec2& CameraPreferences::getResolution() const
{
    return resolution;
}

void CameraPreferences::setResolution(const glm::vec2& res)
{
    resolution = res;
}

bool CameraPreferences::isResetRequested() const
{
    return resetRequested;
}

void CameraPreferences::requestReset()
{
    resetRequested = true;
}

void CameraPreferences::clearResetRequested()
{
    resetRequested = false;
}

bool CameraPreferences::hasModeChangeRequest() const
{
    return modeChangeRequested;
}

int CameraPreferences::getRequestedMode() const
{
    return requestedMode;
}

void CameraPreferences::requestModeChange(int mode)
{
    cameraMode          = mode;
    requestedMode       = mode;
    modeChangeRequested = true;
}

void CameraPreferences::clearModeChangeRequest()
{
    modeChangeRequested = false;
}
