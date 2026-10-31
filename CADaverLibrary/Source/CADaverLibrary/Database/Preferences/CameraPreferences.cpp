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
    jsonObject["orthographic"]        = orthographic;
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

    if (jsonObject.contains("orthographic"))
    {
        orthographic = jsonObject.at("orthographic").get<bool>();
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

bool CameraPreferences::isOrthographic() const
{
    return orthographic;
}

void CameraPreferences::setOrthographic(bool orthographicInput)
{
    orthographic = orthographicInput;
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

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("CameraPreferences/Serialization")
{
    CameraPreferences prefs;
    prefs.setCameraMode(1);
    prefs.setCameraSpeed(12.5f);
    prefs.setRotationSensitivity(0.01);
    prefs.setFov(1.2f);
    prefs.setNearPlane(0.1f);
    prefs.setFarPlane(1000.0f);
    prefs.setOrthographic(true);

    nlohmann::json json = prefs.toJson();
    CHECK(json["orthographic"] == true);
    CHECK(json["cameraMode"] == 1);

    CameraPreferences loaded;
    loaded.fromJson(json);
    CHECK(loaded.isOrthographic() == true);
    CHECK(loaded.getCameraMode() == 1);
    CHECK(loaded.getCameraSpeed() == 12.5f);
}
#endif
