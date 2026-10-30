#include "Camera/CameraController.h"

#include "CADacomb/Volume/AABB3.h"
#include "Database/Asset/CadAsset.h"
#include "Database/Asset/TriangulationAsset.h"
#include "Database/Asset/VoxelAsset.h"
#include "Database/Preferences/CameraPreferences.h"
#include "Database/Project.h"
#include "Vulcant/Interface/VulcantInputValue.h"
#include "Vulcant/Interface/VulcantWindow.h"
#include "Vulcant/Toolkit/ArcCam.h"
#include "Vulcant/Toolkit/Camera.h"
#include "Vulcant/Toolkit/Freecam.h"

#include <algorithm>

CameraController::CameraController() {}

CameraController::~CameraController() {}

void CameraController::setMode(int mode, Vulcant::VulcantWindow& window, const CameraPreferences& camPrefs, const glm::ivec2& resolution)
{
    bool       wasOrtho = camera ? camera->isOrthographic() : false;
    glm::dvec3 eye      = camera ? camera->getEye() : glm::dvec3(0.0, 3.0, 5.0);
    glm::dvec3 target   = camera ? camera->getTarget() : glm::dvec3(0.0, 0.0, 0.0);
    glm::dvec3 up       = camera ? camera->getUp() : glm::dvec3(0.0, 1.0, 0.0);
    float      zNear    = camPrefs.getNearPlane();
    float      zFar     = camPrefs.getFarPlane();
    float      fov      = camPrefs.getFov();
    float      aspect   = camera ? camera->getAspect() : ((resolution.y > 0) ? static_cast<float>(resolution.x) / static_cast<float>(resolution.y) : 16.0f / 9.0f);
    double     speed    = static_cast<double>(camPrefs.getCameraSpeed());
    double     rotSpeed = camPrefs.getRotationSensitivity();

    if (mode == 1)
    {
        camera = std::make_unique<Vulcant::Toolkit::Freecam>(window, eye, target, up);
    }
    else
    {
        camera = std::make_unique<Vulcant::Toolkit::ArcCam>(window, eye, target, up);
    }

    currentCameraMode = mode;

    camera->setZNear(zNear);
    camera->setZFar(zFar);
    camera->setFov(fov);
    camera->setAspect(aspect);
    camera->setTranslationSensitivity(speed);
    camera->setRotationSensitivity(rotSpeed);
    camera->setOrthographic(wasOrtho);
}

int CameraController::getMode() const
{
    return currentCameraMode;
}

void CameraController::updateAspect(const glm::ivec2& resolution)
{
    if (camera && resolution.y > 0)
    {
        camera->setAspect(static_cast<float>(resolution.x) / static_cast<float>(resolution.y));
    }
}

void CameraController::tick(double delta)
{
    if (camera)
    {
        camera->tick(delta);
    }
}

void CameraController::keyEvent(const Vulcant::VulcantInputValue& key)
{
    if (camera)
    {
        camera->keyEvent(key);
    }
}

void CameraController::frameOnProject(const Project& project)
{
    CADacomb::AABB3 globalAABB;
    bool            hasBounds = false;

    for (const auto& asset : project.getAssets())
    {
        if (asset && asset->getType() == "Triangulation")
        {
            auto triAsset = std::dynamic_pointer_cast<const TriangulationAsset>(asset);
            if (triAsset && triAsset->getTriangulation() && !triAsset->getTriangulation()->vertices.empty())
            {
                auto [pos, size] = triAsset->getTriangulation()->getAABB();
                CADacomb::AABB3 assetAABB(pos, pos + size);
                if (!hasBounds)
                {
                    globalAABB = assetAABB;
                    hasBounds  = true;
                }
                else
                {
                    globalAABB.expand(assetAABB);
                }
            }
        }
        else if (asset && asset->getType() == "Cad")
        {
            auto cadAsset = std::dynamic_pointer_cast<const CadAsset>(asset);
            if (cadAsset && cadAsset->getTriangulation() && !cadAsset->getTriangulation()->vertices.empty())
            {
                auto [pos, size] = cadAsset->getTriangulation()->getAABB();
                CADacomb::AABB3 assetAABB(pos, pos + size);
                if (!hasBounds)
                {
                    globalAABB = assetAABB;
                    hasBounds  = true;
                }
                else
                {
                    globalAABB.expand(assetAABB);
                }
            }
        }
        else if (asset && asset->getType() == "Voxel")
        {
            auto voxelAsset = std::dynamic_pointer_cast<const VoxelAsset>(asset);
            if (voxelAsset && voxelAsset->getSVO())
            {
                if (!hasBounds)
                {
                    globalAABB = voxelAsset->getAABB();
                    hasBounds  = true;
                }
                else
                {
                    globalAABB.expand(voxelAsset->getAABB());
                }
            }
        }
    }

    if (hasBounds && camera)
    {
        glm::dvec3 center = globalAABB.getCenter();
        glm::dvec3 size   = globalAABB.getSize();
        double     maxDim = std::max({ size.x, size.y, size.z });
        double     dist   = (maxDim > 1e-4) ? maxDim * 1.8 : 5.0;

        glm::dvec3 curDir = camera->getEye() - camera->getTarget();
        if (glm::length(curDir) > 1e-6)
        {
            curDir = glm::normalize(curDir);
        }
        else
        {
            curDir = glm::normalize(glm::dvec3(0.0, 3.0, 5.0));
        }

        camera->setTarget(center);
        camera->setEye(center + curDir * dist);
        camera->setZNear(static_cast<float>(std::max(0.01, dist * 0.005)));
        camera->setZFar(static_cast<float>(std::max(2000.0, dist * 50.0)));
    }
    else if (camera)
    {
        glm::dvec3 curDir = camera->getEye() - camera->getTarget();
        if (glm::length(curDir) > 1e-6)
        {
            curDir = glm::normalize(curDir);
        }
        else
        {
            curDir = glm::normalize(glm::dvec3(0.0, 3.0, 5.0));
        }

        camera->setTarget(glm::dvec3(0.0, 0.0, 0.0));
        camera->setEye(curDir * 5.0);
    }
}

void CameraController::reset(const CameraPreferences& camPrefs, const Project& project)
{
    if (project.getAssetCount() > 0)
    {
        frameOnProject(project);
    }
    else if (camera)
    {
        camera->setTarget(glm::dvec3(0.0, 0.0, 0.0));
        camera->setEye(glm::dvec3(0.0, 3.0, 5.0));
        camera->setUp(glm::dvec3(0.0, 1.0, 0.0));
        camera->setZNear(camPrefs.getNearPlane());
        camera->setZFar(camPrefs.getFarPlane());
        camera->setFov(camPrefs.getFov());
    }
}

bool CameraController::isOrthographic() const
{
    return camera ? camera->isOrthographic() : false;
}

void CameraController::setOrthographic(bool ortho)
{
    if (camera)
    {
        camera->setOrthographic(ortho);
    }
}

void CameraController::toggleOrthographic()
{
    if (camera)
    {
        camera->setOrthographic(!camera->isOrthographic());
    }
}

Vulcant::Toolkit::Camera* CameraController::getCamera() const
{
    return camera.get();
}

Vulcant::Toolkit::SceneData CameraController::getSceneData() const
{
    if (camera)
    {
        return camera->getScene();
    }
    return Vulcant::Toolkit::SceneData{};
}
