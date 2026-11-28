#pragma once

#include "Polyglot/Node.h"

#include <nlohmann/json.hpp>

// Holds sphere-tracing and shading preferences for SDF rendering.
class SdfPreferences
{
  public:
    SdfPreferences();
    virtual ~SdfPreferences();

    Polyglot::Node toNode() const;
    void           fromNode(const Polyglot::Node& node);

    nlohmann::json toJson() const;
    void           fromJson(const nlohmann::json& jsonObject);

    int  getMaxSteps() const;
    void setMaxSteps(int steps);

    double getHitEpsilon() const;
    void   setHitEpsilon(double epsilon);

    double getStepScale() const;
    void   setStepScale(double scale);

    double getNormalEpsilon() const;
    void   setNormalEpsilon(double epsilon);

    double getAmbientStrength() const;
    void   setAmbientStrength(double strength);

  private:
    double hitEpsilon      = 0.0005;
    double stepScale       = 1.0;
    double normalEpsilon   = 0.001;
    double ambientStrength = 0.25;
    int    maxSteps        = 128;
};
