#include "Database/Preferences/SdfPreferences.h"

#include <stdexcept>

SdfPreferences::SdfPreferences() {}

SdfPreferences::~SdfPreferences() {}

Polyglot::Node SdfPreferences::toNode() const
{
    return Polyglot::Node(toJson());
}

void SdfPreferences::fromNode(const Polyglot::Node& node)
{
    fromJson(node.toJson());
}

nlohmann::json SdfPreferences::toJson() const
{
    nlohmann::json jsonObject;
    jsonObject["Version"]         = 1;
    jsonObject["maxSteps"]        = maxSteps;
    jsonObject["hitEpsilon"]      = hitEpsilon;
    jsonObject["stepScale"]       = stepScale;
    jsonObject["normalEpsilon"]   = normalEpsilon;
    jsonObject["ambientStrength"] = ambientStrength;
    return jsonObject;
}

void SdfPreferences::fromJson(const nlohmann::json& jsonObject)
{
    int version = jsonObject.at("Version").get<int>();
    if (version != 1)
    {
        throw std::runtime_error("Unsupported SdfPreferences version");
    }

    maxSteps        = jsonObject.at("maxSteps").get<int>();
    hitEpsilon      = jsonObject.at("hitEpsilon").get<double>();
    stepScale       = jsonObject.at("stepScale").get<double>();
    normalEpsilon   = jsonObject.at("normalEpsilon").get<double>();
    ambientStrength = jsonObject.at("ambientStrength").get<double>();
}

int SdfPreferences::getMaxSteps() const
{
    return maxSteps;
}

void SdfPreferences::setMaxSteps(int steps)
{
    maxSteps = steps;
}

double SdfPreferences::getHitEpsilon() const
{
    return hitEpsilon;
}

void SdfPreferences::setHitEpsilon(double epsilon)
{
    hitEpsilon = epsilon;
}

double SdfPreferences::getStepScale() const
{
    return stepScale;
}

void SdfPreferences::setStepScale(double scale)
{
    stepScale = scale;
}

double SdfPreferences::getNormalEpsilon() const
{
    return normalEpsilon;
}

void SdfPreferences::setNormalEpsilon(double epsilon)
{
    normalEpsilon = epsilon;
}

double SdfPreferences::getAmbientStrength() const
{
    return ambientStrength;
}

void SdfPreferences::setAmbientStrength(double strength)
{
    ambientStrength = strength;
}

#ifdef ISTESTPROJECT
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("SdfPreferences/JsonRoundTrip")
{
    SdfPreferences prefs;
    prefs.setMaxSteps(256);
    prefs.setHitEpsilon(0.0002);
    prefs.setStepScale(0.75);
    prefs.setNormalEpsilon(0.002);
    prefs.setAmbientStrength(0.4);

    nlohmann::json json = prefs.toJson();
    SdfPreferences loaded;
    loaded.fromJson(json);

    CHECK(loaded.getMaxSteps() == 256);
    CHECK(loaded.getHitEpsilon() == Catch::Approx(0.0002));
    CHECK(loaded.getStepScale() == Catch::Approx(0.75));
    CHECK(loaded.getNormalEpsilon() == Catch::Approx(0.002));
    CHECK(loaded.getAmbientStrength() == Catch::Approx(0.4));
}
#endif
