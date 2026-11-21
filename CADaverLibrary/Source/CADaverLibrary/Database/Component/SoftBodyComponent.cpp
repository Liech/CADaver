#include "Database/Component/SoftBodyComponent.h"

SoftBodyComponent::SoftBodyComponent() {}

SoftBodyComponent::SoftBodyComponent(double massInput,
                                     double pressureInput,
                                     double edgeComplianceInput,
                                     double bendComplianceInput,
                                     double dampingInput,
                                     double frictionInput,
                                     double restitutionInput,
                                     int    solverIterationsInput)
  : massValue(massInput)
  , pressureValue(pressureInput)
  , edgeComplianceValue(edgeComplianceInput)
  , bendComplianceValue(bendComplianceInput)
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
           "  - mass: Total mass of the soft body (kg)\n"
           "  - pressure: Internal gas/volume pressure resistance (Pa)\n"
           "  - edgeCompliance: Surface stretch compliance (inverse stiffness)\n"
           "  - bendCompliance: Surface bending compliance (inverse stiffness)\n"
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
        {          "Version",                     1 },
        {             "mass",             massValue },
        {         "pressure",         pressureValue },
        {   "edgeCompliance",   edgeComplianceValue },
        {   "bendCompliance",   bendComplianceValue },
        {          "damping",          dampingValue },
        {         "friction",         frictionValue },
        {      "restitution",      restitutionValue },
        { "solverIterations", solverIterationsValue }
    };
}

void SoftBodyComponent::fromDataJson(const nlohmann::json& jsonObject)
{
    jsonObject.at("Version").get<int>();
    massValue             = jsonObject.at("mass").get<double>();
    pressureValue         = jsonObject.at("pressure").get<double>();
    edgeComplianceValue   = jsonObject.at("edgeCompliance").get<double>();
    bendComplianceValue   = jsonObject.at("bendCompliance").get<double>();
    dampingValue          = jsonObject.at("damping").get<double>();
    frictionValue         = jsonObject.at("friction").get<double>();
    restitutionValue      = jsonObject.at("restitution").get<double>();
    solverIterationsValue = jsonObject.at("solverIterations").get<int>();
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

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("SoftBodyComponent/BasicPropertiesAndSerialization")
{
    SoftBodyComponent comp(2.5, 1500.0, 0.0005, 0.005, 0.35, 0.45, 0.1, 15);
    CHECK(comp.getType() == "SoftBody");
    CHECK(comp.isCompatible("Triangulation") == true);
    CHECK(comp.isCompatible("Cad") == false);
    CHECK(comp.isCompatible("Voxel") == false);

    CHECK(comp.getMass() == 2.5);
    CHECK(comp.getPressure() == 1500.0);
    CHECK(comp.getEdgeCompliance() == 0.0005);
    CHECK(comp.getBendCompliance() == 0.005);
    CHECK(comp.getDamping() == 0.35);
    CHECK(comp.getFriction() == 0.45);
    CHECK(comp.getRestitution() == 0.1);
    CHECK(comp.getSolverIterations() == 15);

    comp.setPressure(3000.0);
    comp.setSolverIterations(20);
    CHECK(comp.getPressure() == 3000.0);
    CHECK(comp.getSolverIterations() == 20);

    nlohmann::json data = comp.toDataJson();
    CHECK(data.at("Version").get<int>() == 1);
    CHECK(data.at("pressure").get<double>() == 3000.0);
    CHECK(data.at("solverIterations").get<int>() == 20);

    SoftBodyComponent restored;
    restored.fromDataJson(data);
    CHECK(restored.getMass() == 2.5);
    CHECK(restored.getPressure() == 3000.0);
    CHECK(restored.getEdgeCompliance() == 0.0005);
    CHECK(restored.getBendCompliance() == 0.005);
    CHECK(restored.getDamping() == 0.35);
    CHECK(restored.getFriction() == 0.45);
    CHECK(restored.getRestitution() == 0.1);
    CHECK(restored.getSolverIterations() == 20);
}
#endif
