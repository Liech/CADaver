#include "Database/Physics/PhysicsSimulation.h"

#include "CADacomb/Physics/Jolt/PhysicsJ.h"
#include "Database/Asset/TriangulationAsset.h"
#include "Database/Component/RigidBodyComponent.h"
#include "Database/Project.h"

PhysicsSimulation::PhysicsSimulation() {}

PhysicsSimulation::~PhysicsSimulation() {}

void PhysicsSimulation::reset()
{
    bodies.clear();
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

    physics->step(deltaTime);

    for (const auto& pair : bodies)
    {
        auto asset = project.getAsset(pair.first);
        if (asset && pair.second && !pair.second->isStatic())
        {
            asset->setTransformation(pair.second->getTransformation());
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
    physics = std::make_unique<CADacomb::Physics::PhysicsJ>();

    for (const auto& asset : project.getAssets())
    {
        auto triAsset = std::dynamic_pointer_cast<TriangulationAsset>(asset);
        if (!triAsset)
        {
            continue;
        }

        auto rbAttachments = triAsset->getAttachments<RigidBodyComponent>();
        if (rbAttachments.empty())
        {
            continue;
        }

        auto body                   = physics->createRigidBody(triAsset->getTriangulation(), triAsset->getTransformation(), rbAttachments.front()->isStatic());
        bodies[triAsset->getGuid()] = body;
    }
}
