#pragma once

#include "CADacomb/Util/GUID.h"
#include "Rendering/Triangulation/TriangulationSurfaceModule.h"

#include <cstddef>
#include <cstdint>
#include <glm/glm.hpp>
#include <memory>
#include <vector>

namespace Vulcant
{
    class VulcantBuffer;
}

class FaceValuesComponent;

// Surface module evaluating FaceValuesComponent via Lua and uploading Set 1 buffers.
class IndexedFaceColorModule : public TriangulationSurfaceModule
{
  public:
    IndexedFaceColorModule();
    virtual ~IndexedFaceColorModule();

    std::string                                            getModuleKey() const override;
    std::vector<uint32_t>                                  getFragmentSpirv() const override;
    bool                                                   isCompatible(const std::string& componentType) const override;
    void                                                   update(Vulcant::VulcantDevice& device, const TriangulationAsset& asset, const AssetComponent* component) override;
    std::vector<std::shared_ptr<Vulcant::VulcantResource>> getSet1Resources() const override;

  private:
    std::vector<glm::vec4> evaluatePalette(const FaceValuesComponent& faceValues) const;

    std::unique_ptr<Vulcant::VulcantBuffer> faceValueIndicesBuffer;
    std::unique_ptr<Vulcant::VulcantBuffer> paletteColorsBuffer;
    CADacomb::GUID                          lastComponentGuid;
    size_t                                  lastComponentRevision = 0;
    size_t                                  lastAssetRevision     = 0;
    size_t                                  faceIndexCapacity     = 0;
    size_t                                  paletteCapacity       = 0;
};
