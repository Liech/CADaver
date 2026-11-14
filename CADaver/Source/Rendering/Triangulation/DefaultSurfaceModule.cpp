#include "Rendering/Triangulation/DefaultSurfaceModule.h"

#include "Triangulation/Triangulation_Default_frag.h"

DefaultSurfaceModule::DefaultSurfaceModule() {}

DefaultSurfaceModule::~DefaultSurfaceModule() {}

std::string DefaultSurfaceModule::getModuleKey() const
{
    return "Default";
}

std::vector<uint32_t> DefaultSurfaceModule::getFragmentSpirv() const
{
    return std::vector<uint32_t>(Triangulation_Default_frag_spirv, Triangulation_Default_frag_spirv + Triangulation_Default_frag_spirv_sizeInBytes / sizeof(uint32_t));
}

bool DefaultSurfaceModule::isCompatible(const std::string& componentType) const
{
    return componentType.empty();
}

void DefaultSurfaceModule::update(Vulcant::VulcantDevice&, const TriangulationAsset&, const AssetComponent*) {}

std::vector<std::shared_ptr<Vulcant::VulcantResource>> DefaultSurfaceModule::getSet1Resources() const
{
    return {};
}
