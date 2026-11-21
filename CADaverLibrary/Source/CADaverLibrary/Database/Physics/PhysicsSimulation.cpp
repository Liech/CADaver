#include "Database/Physics/PhysicsSimulation.h"

#include "CADacomb/Physics/Jolt/PhysicsJ.h"
#include "Database/Asset/CadAsset.h"
#include "Database/Asset/TriangulationAsset.h"
#include "Database/Asset/VoxelAsset.h"
#include "Database/Component/RigidBodyComponent.h"
#include "Database/Project.h"

PhysicsSimulation::PhysicsSimulation() {}

PhysicsSimulation::~PhysicsSimulation() {}

void PhysicsSimulation::reset()
{
    bodies.clear();
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
    assetRevisions.clear();
    physics = std::make_unique<CADacomb::Physics::PhysicsJ>();

    for (const auto& asset : project.getAssets())
    {
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
