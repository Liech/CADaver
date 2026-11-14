#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace Vulcant
{
    class VulcantDevice;
    class VulcantResource;
}

class AssetComponent;
class TriangulationAsset;

// Abstract C++ counterpart to Slang ISurfaceModifier modules for triangulation rendering.
class TriangulationSurfaceModule
{
  public:
    TriangulationSurfaceModule();
    virtual ~TriangulationSurfaceModule();

    virtual std::string                                    getModuleKey() const                                                                                       = 0;
    virtual std::vector<uint32_t>                          getFragmentSpirv() const                                                                                   = 0;
    virtual bool                                           isCompatible(const std::string& componentType) const                                                       = 0;
    virtual void                                           update(Vulcant::VulcantDevice& device, const TriangulationAsset& asset, const AssetComponent* component) = 0;
    virtual std::vector<std::shared_ptr<Vulcant::VulcantResource>> getSet1Resources() const                                                                           = 0;
};
