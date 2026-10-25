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
    if (jsonObject.contains("cameraMode") && jsonObject["cameraMode"].is_number_integer())
    {
        cameraMode = jsonObject["cameraMode"].get<int>();
    }

    if (jsonObject.contains("cameraSpeed") && jsonObject["cameraSpeed"].is_number())
    {
        cameraSpeed = jsonObject["cameraSpeed"].get<float>();
    }

    if (jsonObject.contains("rotationSensitivity") && jsonObject["rotationSensitivity"].is_number())
    {
        rotationSensitivity = jsonObject["rotationSensitivity"].get<double>();
    }

    if (jsonObject.contains("fov") && jsonObject["fov"].is_number())
    {
        fov = jsonObject["fov"].get<float>();
    }

    if (jsonObject.contains("nearPlane") && jsonObject["nearPlane"].is_number())
    {
        nearPlane = jsonObject["nearPlane"].get<float>();
    }

    if (jsonObject.contains("farPlane") && jsonObject["farPlane"].is_number())
    {
        farPlane = jsonObject["farPlane"].get<float>();
    }

    if (jsonObject.contains("resolution") && jsonObject["resolution"].is_array() && jsonObject["resolution"].size() >= 2)
    {
        resolution.x = jsonObject["resolution"][0].get<float>();
        resolution.y = jsonObject["resolution"][1].get<float>();
    }
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
