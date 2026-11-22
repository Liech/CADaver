#include "Database/Physics/PhysicsSimulation.h"

#include "CADacomb/Physics/Jolt/PhysicsJ.h"
#include "Database/Asset/CadAsset.h"
#include "Database/Asset/TriangulationAsset.h"
#include "Database/Asset/VoxelAsset.h"
#include "Database/Component/RigidBodyComponent.h"
#include "Database/Component/SoftBodyComponent.h"
#include "Database/Project.h"

PhysicsSimulation::PhysicsSimulation() {}

PhysicsSimulation::~PhysicsSimulation() {}

void PhysicsSimulation::reset()
{
    bodies.clear();
    softBodies.clear();
    assetRevisions.clear();
    physics.reset();
    activeSessionGuid = CADacomb::GUID("00000000-0000-0000-0000-000000000000");
}

void PhysicsSimulation::step(Project& project, double deltaTime, const CADacomb::GUID& sessionMergeGuid)
{
    if (!physics || isZeroGuid(sessionMergeGuid) || activeSessionGuid != sessionMergeGuid)
    {
        rebuildWorld(project);
        activeSessionGuid = sessionMergeGuid;
    }
    else
    {
        for (const auto& pair : bodies)
        {
            auto asset = project.getAsset(pair.first);
            if (asset && pair.second && asset->getRevision() != assetRevisions[pair.first])
            {
                pair.second->notifyGeometryChanged();
                assetRevisions[pair.first] = asset->getRevision();
            }
        }
    }

    physics->step(deltaTime);

    for (const auto& pair : bodies)
    {
        auto asset = project.getAsset(pair.first);
        if (asset && pair.second && !pair.second->isStatic())
        {
            asset->setTransformation(pair.second->getTransformation());
            assetRevisions[pair.first] = asset->getRevision();
        }
    }

    for (const auto& pair : softBodies)
    {
        auto triAsset = std::dynamic_pointer_cast<TriangulationAsset>(project.getAsset(pair.first));
        if (triAsset && pair.second)
        {
            triAsset->setTransformation(pair.second->getTransformation());
            std::vector<glm::dvec3> deformedVertices = pair.second->getDeformedVertices();
            if (deformedVertices.size() == triAsset->getTriangulation().vertices.size())
            {
                triAsset->getTriangulation().vertices = std::move(deformedVertices);
                triAsset->setMeshColor(triAsset->getMeshColor());
            }
            assetRevisions[pair.first] = triAsset->getRevision();
        }
    }
}

bool PhysicsSimulation::isZeroGuid(const CADacomb::GUID& guid) const
{
    for (char c : guid.toString())
    {
        if (c != '0' && c != '-')
        {
            return false;
        }
    }
    return true;
}

void PhysicsSimulation::rebuildWorld(Project& project)
{
    bodies.clear();
    softBodies.clear();
    assetRevisions.clear();
    physics = std::make_unique<CADacomb::Physics::PhysicsJ>();

    for (const auto& asset : project.getAssets())
    {
        auto sbAttachments = asset->getAttachments<SoftBodyComponent>();
        if (!sbAttachments.empty())
        {
            if (auto triAsset = std::dynamic_pointer_cast<TriangulationAsset>(asset))
            {
                const auto& sbComp               = sbAttachments.front();
                auto        sb                   = physics->createSoftBody(triAsset->getTriangulation(),
                                                                           triAsset->getTransformation(),
                                                                           sbComp->getMode(),
                                                                           sbComp->getMass(),
                                                                           sbComp->getPressure(),
                                                                           sbComp->getEdgeCompliance(),
                                                                           sbComp->getBendCompliance(),
                                                                           sbComp->getVolumeCompliance(),
                                                                           sbComp->getInternalResolution(),
                                                                           sbComp->getDamping(),
                                                                           sbComp->getFriction(),
                                                                           sbComp->getRestitution(),
                                                                           sbComp->getSolverIterations());
                softBodies[triAsset->getGuid()]  = sb;
                assetRevisions[asset->getGuid()] = asset->getRevision();
                continue;
            }
        }

        auto rbAttachments = asset->getAttachments<RigidBodyComponent>();
        if (rbAttachments.empty())
        {
            continue;
        }

        bool isStatic = rbAttachments.front()->isStatic();

        if (auto voxelAsset = std::dynamic_pointer_cast<VoxelAsset>(asset))
        {
            auto body                        = physics->createRigidBody(voxelAsset->getSVO(), voxelAsset->getAABB(), voxelAsset->getTransformation(), isStatic);
            bodies[voxelAsset->getGuid()]    = body;
            assetRevisions[asset->getGuid()] = asset->getRevision();
            continue;
        }

        const CADacomb::Triangulation* triangulation = nullptr;
        if (auto triAsset = std::dynamic_pointer_cast<TriangulationAsset>(asset))
        {
            triangulation = &triAsset->getTriangulation();
        }
        else if (auto cadAsset = std::dynamic_pointer_cast<CadAsset>(asset))
        {
            triangulation = &cadAsset->getTriangulation();
        }

        if (!triangulation)
        {
            continue;
        }

        auto body                        = physics->createRigidBody(*triangulation, asset->getTransformation(), isStatic);
        bodies[asset->getGuid()]         = body;
        assetRevisions[asset->getGuid()] = asset->getRevision();
    }
}
