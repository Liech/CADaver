#pragma once

#include "Rendering/Triangulation/TriangulationSurfaceModule.h"

// Default surface module rendering base triangulation color without Set 1 resources.
class DefaultSurfaceModule : public TriangulationSurfaceModule
{
  public:
    DefaultSurfaceModule();
    virtual ~DefaultSurfaceModule();

    std::string                                            getModuleKey() const override;
    std::vector<uint32_t>                                  getFragmentSpirv() const override;
    bool                                                   isCompatible(const std::string& componentType) const override;
    void                                                   update(Vulcant::VulcantDevice& device, const TriangulationAsset& asset, const AssetComponent* component) override;
    std::vector<std::shared_ptr<Vulcant::VulcantResource>> getSet1Resources() const override;
};
