#pragma once

#include "Database/AssetComponent.h"

#include <string>

// RigidBody component attached to a TriangulationAsset to enable physics simulation.
class RigidBodyComponent : public AssetComponent
{
  public:
    RigidBodyComponent();
    explicit RigidBodyComponent(bool isStatic);
    virtual ~RigidBodyComponent();

    std::string getType() const override;
    std::string getDescription() const override;
    size_t      getSizeInBytes() const override;
    bool        isCompatible(const std::string& assetType) const override;

    nlohmann::json toDataJson() const override;
    void           fromDataJson(const nlohmann::json& jsonObject) override;

    bool isStatic() const;
    void setStatic(bool isStatic);

  private:
    bool staticBody = false;
};
