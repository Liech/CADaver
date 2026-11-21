#pragma once

#include "CADacomb/Physics/Interface/Physics.h"
#include "CADacomb/Physics/Interface/RigidBody.h"
#include "CADacomb/Physics/Interface/SoftBody.h"
#include "CADacomb/Util/GUID.h"

#include <cstddef>
#include <map>
#include <memory>

class Project;

// Manages the active physics engine session for stepping project assets with RigidBody or SoftBody components.
class PhysicsSimulation
{
  public:
    PhysicsSimulation();
    virtual ~PhysicsSimulation();

    void reset();
    void step(Project& project, double deltaTime, const CADacomb::GUID& sessionMergeGuid);

  private:
    bool isZeroGuid(const CADacomb::GUID& guid) const;
    void rebuildWorld(Project& project);

    std::unique_ptr<CADacomb::Physics::Physics>                             physics;
    std::map<CADacomb::GUID, std::shared_ptr<CADacomb::Physics::RigidBody>> bodies;
    std::map<CADacomb::GUID, std::shared_ptr<CADacomb::Physics::SoftBody>>  softBodies;
    std::map<CADacomb::GUID, size_t>                                        assetRevisions;
    CADacomb::GUID                                                          activeSessionGuid;
};
