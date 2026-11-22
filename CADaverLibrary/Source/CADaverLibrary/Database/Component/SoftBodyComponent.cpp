#include "Database/Component/SoftBodyComponent.h"

SoftBodyComponent::SoftBodyComponent() {}

SoftBodyComponent::SoftBodyComponent(CADacomb::Physics::SoftBodyMode modeInput,
                                     double                          massInput,
                                     double                          pressureInput,
                                     double                          edgeComplianceInput,
                                     double                          bendComplianceInput,
                                     double                          volumeComplianceInput,
                                     int                             internalResolutionInput,
                                     double                          dampingInput,
                                     double                          frictionInput,
                                     double                          restitutionInput,
                                     int                             solverIterationsInput)
  : modeValue(modeInput)
  , massValue(massInput)
  , pressureValue(pressureInput)
  , edgeComplianceValue(edgeComplianceInput)
  , bendComplianceValue(bendComplianceInput)
  , volumeComplianceValue(volumeComplianceInput)
  , internalResolutionValue(internalResolutionInput)
  , dampingValue(dampingInput)
  , frictionValue(frictionInput)
  , restitutionValue(restitutionInput)
  , solverIterationsValue(solverIterationsInput)
{
}

SoftBodyComponent::~SoftBodyComponent() {}

std::string SoftBodyComponent::getType() const
{
    return "SoftBody";
}

std::string SoftBodyComponent::getDescription() const
{
    return "Component: SoftBody\n"
           "Purpose: Attaches deformable XPBD soft body physics properties to a triangulation asset.\n"
           "Compatibility: Compatible exclusively with Triangulation assets.\n\n"
           "Data Members:\n"
           "  - mode: Soft body constraint topology (Balloon, InternalStruts, or Volumetric)\n"
           "  - mass: Total mass of the soft body (kg)\n"
           "  - pressure: Internal gas pressure resistance (Pa, used in Balloon mode)\n"
           "  - edgeCompliance: Surface stretch compliance (inverse stiffness)\n"
           "  - bendCompliance: Surface bending compliance (inverse stiffness)\n"
           "  - volumeCompliance: Internal strut and tetrahedral volume compliance (used in InternalStruts and Volumetric modes)\n"
           "  - internalResolution: Voxel grid resolution along longest axis for Volumetric interior lattice\n"
           "  - damping: Linear velocity damping\n"
           "  - friction: Surface contact friction\n"
           "  - restitution: Surface contact bounciness [0..1]\n"
           "  - solverIterations: XPBD constraint solver iterations per step";
}

size_t SoftBodyComponent::getSizeInBytes() const
{
    return sizeof(SoftBodyComponent);
}

bool SoftBodyComponent::isCompatible(const std::string& assetType) const
{
    return assetType == "Triangulation";
}

nlohmann::json SoftBodyComponent::toDataJson() const
{
    return {
        {            "Version",                       2 },
        {               "mode", modeToString(modeValue) },
        {               "mass",               massValue },
        {           "pressure",           pressureValue },
        {     "edgeCompliance",     edgeComplianceValue },
        {     "bendCompliance",     bendComplianceValue },
        {   "volumeCompliance",   volumeComplianceValue },
        { "internalResolution", internalResolutionValue },
        {            "damping",            dampingValue },
        {           "friction",           frictionValue },
        {        "restitution",        restitutionValue },
        {   "solverIterations",   solverIterationsValue }
    };
}

void SoftBodyComponent::fromDataJson(const nlohmann::json& jsonObject)
{
    jsonObject.at("Version").get<int>();
    modeValue               = modeFromString(jsonObject.at("mode").get<std::string>());
    massValue               = jsonObject.at("mass").get<double>();
    pressureValue           = jsonObject.at("pressure").get<double>();
    edgeComplianceValue     = jsonObject.at("edgeCompliance").get<double>();
    bendComplianceValue     = jsonObject.at("bendCompliance").get<double>();
    volumeComplianceValue   = jsonObject.at("volumeCompliance").get<double>();
    internalResolutionValue = jsonObject.at("internalResolution").get<int>();
    dampingValue            = jsonObject.at("damping").get<double>();
    frictionValue           = jsonObject.at("friction").get<double>();
    restitutionValue        = jsonObject.at("restitution").get<double>();
    solverIterationsValue   = jsonObject.at("solverIterations").get<int>();
    markModified();
}

CADacomb::Physics::SoftBodyMode SoftBodyComponent::getMode() const
{
    return modeValue;
}

void SoftBodyComponent::setMode(CADacomb::Physics::SoftBodyMode modeInput)
{
    modeValue = modeInput;
    markModified();
}

double SoftBodyComponent::getMass() const
{
    return massValue;
}

void SoftBodyComponent::setMass(double massInput)
{
    massValue = massInput;
    markModified();
}

double SoftBodyComponent::getPressure() const
{
    return pressureValue;
}

void SoftBodyComponent::setPressure(double pressureInput)
{
    pressureValue = pressureInput;
    markModified();
}

double SoftBodyComponent::getEdgeCompliance() const
{
    return edgeComplianceValue;
}

void SoftBodyComponent::setEdgeCompliance(double edgeComplianceInput)
{
    edgeComplianceValue = edgeComplianceInput;
    markModified();
}

double SoftBodyComponent::getBendCompliance() const
{
    return bendComplianceValue;
}

void SoftBodyComponent::setBendCompliance(double bendComplianceInput)
{
    bendComplianceValue = bendComplianceInput;
    markModified();
}

double SoftBodyComponent::getVolumeCompliance() const
{
    return volumeComplianceValue;
}

void SoftBodyComponent::setVolumeCompliance(double volumeComplianceInput)
{
    volumeComplianceValue = volumeComplianceInput;
    markModified();
}

int SoftBodyComponent::getInternalResolution() const
{
    return internalResolutionValue;
}

void SoftBodyComponent::setInternalResolution(int internalResolutionInput)
{
    internalResolutionValue = internalResolutionInput;
    markModified();
}

double SoftBodyComponent::getDamping() const
{
    return dampingValue;
}

void SoftBodyComponent::setDamping(double dampingInput)
{
    dampingValue = dampingInput;
    markModified();
}

double SoftBodyComponent::getFriction() const
{
    return frictionValue;
}

void SoftBodyComponent::setFriction(double frictionInput)
{
    frictionValue = frictionInput;
    markModified();
}

double SoftBodyComponent::getRestitution() const
{
    return restitutionValue;
}

void SoftBodyComponent::setRestitution(double restitutionInput)
{
    restitutionValue = restitutionInput;
    markModified();
}

int SoftBodyComponent::getSolverIterations() const
{
    return solverIterationsValue;
}

void SoftBodyComponent::setSolverIterations(int solverIterationsInput)
{
    solverIterationsValue = solverIterationsInput;
    markModified();
}

std::string SoftBodyComponent::modeToString(CADacomb::Physics::SoftBodyMode mode)
{
    if (mode == CADacomb::Physics::SoftBodyMode::InternalStruts)
    {
        return "InternalStruts";
    }
    if (mode == CADacomb::Physics::SoftBodyMode::Volumetric)
    {
        return "Volumetric";
    }
    return "Balloon";
}

CADacomb::Physics::SoftBodyMode SoftBodyComponent::modeFromString(const std::string& modeName)
{
    if (modeName == "InternalStruts")
    {
        return CADacomb::Physics::SoftBodyMode::InternalStruts;
    }
    if (modeName == "Volumetric")
    {
        return CADacomb::Physics::SoftBodyMode::Volumetric;
    }
    return CADacomb::Physics::SoftBodyMode::Balloon;
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("SoftBodyComponent/BasicPropertiesAndSerialization")
{
    SoftBodyComponent comp(CADacomb::Physics::SoftBodyMode::InternalStruts, 2.5, 1500.0, 0.0005, 0.005, 0.0003, 6, 0.35, 0.45, 0.1, 15);
    CHECK(comp.getType() == "SoftBody");
    CHECK(comp.isCompatible("Triangulation") == true);
    CHECK(comp.isCompatible("Cad") == false);
    CHECK(comp.isCompatible("Voxel") == false);

    CHECK(comp.getMode() == CADacomb::Physics::SoftBodyMode::InternalStruts);
    CHECK(comp.getMass() == 2.5);
    CHECK(comp.getPressure() == 1500.0);
    CHECK(comp.getEdgeCompliance() == 0.0005);
    CHECK(comp.getBendCompliance() == 0.005);
    CHECK(comp.getVolumeCompliance() == 0.0003);
    CHECK(comp.getInternalResolution() == 6);
    CHECK(comp.getDamping() == 0.35);
    CHECK(comp.getFriction() == 0.45);
    CHECK(comp.getRestitution() == 0.1);
    CHECK(comp.getSolverIterations() == 15);

    comp.setMode(CADacomb::Physics::SoftBodyMode::Volumetric);
    comp.setVolumeCompliance(0.0001);
    comp.setInternalResolution(8);
    CHECK(comp.getMode() == CADacomb::Physics::SoftBodyMode::Volumetric);
    CHECK(comp.getVolumeCompliance() == 0.0001);
    CHECK(comp.getInternalResolution() == 8);

    nlohmann::json data = comp.toDataJson();
    CHECK(data.at("Version").get<int>() == 2);
    CHECK(data.at("mode").get<std::string>() == "Volumetric");
    CHECK(data.at("volumeCompliance").get<double>() == 0.0001);
    CHECK(data.at("internalResolution").get<int>() == 8);

    SoftBodyComponent restored;
    restored.fromDataJson(data);
    CHECK(restored.getMode() == CADacomb::Physics::SoftBodyMode::Volumetric);
    CHECK(restored.getMass() == 2.5);
    CHECK(restored.getPressure() == 1500.0);
    CHECK(restored.getEdgeCompliance() == 0.0005);
    CHECK(restored.getBendCompliance() == 0.005);
    CHECK(restored.getVolumeCompliance() == 0.0001);
    CHECK(restored.getInternalResolution() == 8);
    CHECK(restored.getDamping() == 0.35);
    CHECK(restored.getFriction() == 0.45);
    CHECK(restored.getRestitution() == 0.1);
    CHECK(restored.getSolverIterations() == 15);
}
#endif
