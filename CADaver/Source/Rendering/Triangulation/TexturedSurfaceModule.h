#pragma once

#include "CADacomb/Util/GUID.h"
#include "Rendering/Triangulation/TriangulationSurfaceModule.h"

#include <cstddef>
#include <memory>

namespace Vulcant
{
    class VulcantBuffer;
    class VulcantImage;
}

// Surface module uploading per-face-corner UVs and diffuse Texture for TexturedSurfaceModifier.
class TexturedSurfaceModule : public TriangulationSurfaceModule
{
  public:
    TexturedSurfaceModule();
    virtual ~TexturedSurfaceModule();

    std::string                                            getModuleKey() const override;
    std::vector<uint32_t>                                  getFragmentSpirv() const override;
    bool                                                   isCompatible(const std::string& componentType) const override;
    void                                                   update(Vulcant::VulcantDevice& device, const TriangulationAsset& asset, const AssetComponent* component) override;
    std::vector<std::shared_ptr<Vulcant::VulcantResource>> getSet1Resources() const override;

  private:
    std::unique_ptr<Vulcant::VulcantBuffer> uvBuffer;
    std::unique_ptr<Vulcant::VulcantImage>  textureImage;
    size_t                                  uvCapacity = 0;
    CADacomb::GUID                          lastComponentGuid;
    size_t                                  lastComponentRevision = 0;
    CADacomb::GUID                          lastUvComponentGuid;
    size_t                                  lastUvComponentRevision = 0;
    size_t                                  lastAssetRevision       = 0;
};
