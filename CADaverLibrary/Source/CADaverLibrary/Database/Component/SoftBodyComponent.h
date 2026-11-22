#pragma once

#include "CADacomb/Physics/SoftBodyMode.h"
#include "Database/AssetComponent.h"

#include <string>

// SoftBody component attached to a TriangulationAsset to enable deformable soft body physics simulation.
class SoftBodyComponent : public AssetComponent
{
  public:
    SoftBodyComponent();
    SoftBodyComponent(CADacomb::Physics::SoftBodyMode mode,
                      double                          mass,
                      double                          pressure,
                      double                          edgeCompliance,
                      double                          bendCompliance,
                      double                          volumeCompliance,
                      int                             internalResolution,
                      double                          damping,
                      double                          friction,
                      double                          restitution,
                      int                             solverIterations);
    virtual ~SoftBodyComponent();

    std::string getType() const override;
    std::string getDescription() const override;
    size_t      getSizeInBytes() const override;
    bool        isCompatible(const std::string& assetType) const override;

    nlohmann::json toDataJson() const override;
    void           fromDataJson(const nlohmann::json& jsonObject) override;

    CADacomb::Physics::SoftBodyMode getMode() const;
    void                            setMode(CADacomb::Physics::SoftBodyMode mode);
    double                          getMass() const;
    void                            setMass(double mass);
    double                          getPressure() const;
    void                            setPressure(double pressure);
    double                          getEdgeCompliance() const;
    void                            setEdgeCompliance(double edgeCompliance);
    double                          getBendCompliance() const;
    void                            setBendCompliance(double bendCompliance);
    double                          getVolumeCompliance() const;
    void                            setVolumeCompliance(double volumeCompliance);
    int                             getInternalResolution() const;
    void                            setInternalResolution(int internalResolution);
    double                          getDamping() const;
    void                            setDamping(double damping);
    double                          getFriction() const;
    void                            setFriction(double friction);
    double                          getRestitution() const;
    void                            setRestitution(double restitution);
    int                             getSolverIterations() const;
    void                            setSolverIterations(int solverIterations);

  private:
    static std::string                     modeToString(CADacomb::Physics::SoftBodyMode mode);
    static CADacomb::Physics::SoftBodyMode modeFromString(const std::string& modeName);

    CADacomb::Physics::SoftBodyMode modeValue               = CADacomb::Physics::SoftBodyMode::Volumetric;
    double                          massValue               = 1.0;
    double                          pressureValue           = 2000.0;
    double                          edgeComplianceValue     = 0.0001;
    double                          bendComplianceValue     = 0.001;
    double                          volumeComplianceValue   = 0.0002;
    int                             internalResolutionValue = 5;
    double                          dampingValue            = 0.25;
    double                          frictionValue           = 0.4;
    double                          restitutionValue        = 0.0;
    int                             solverIterationsValue   = 12;
};
