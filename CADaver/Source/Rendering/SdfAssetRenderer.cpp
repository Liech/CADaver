#include "Rendering/SdfAssetRenderer.h"

#include "Database/Asset/SdfAsset.h"
#include "Database/Preferences/SdfPreferences.h"
#include "Execution/GatedDatabase.h"
#include "Rendering/Renderer.h"

#include <glm/glm.hpp>
#include <iostream>
#include <string>

SdfAssetRenderer::SdfAssetRenderer() {}

SdfAssetRenderer::SdfAssetRenderer(std::shared_ptr<const SdfAsset> assetInput)
  : asset(std::move(assetInput))
{
}

SdfAssetRenderer::~SdfAssetRenderer() {}

void SdfAssetRenderer::setAsset(const std::shared_ptr<const Asset>& assetInput)
{
    auto sdfAsset = std::dynamic_pointer_cast<const SdfAsset>(assetInput);
    if (sdfAsset != asset)
    {
        asset = std::move(sdfAsset);
        lastFormulaName.clear();
        lastFormulaHelper.clear();
        lastFormulaExpression.clear();
        dirty = true;
    }
}

void SdfAssetRenderer::render(Renderer& renderer, GatedDatabase& gatedDatabase, const Vulcant::Toolkit::SceneData& sceneData)
{
    if (!asset)
    {
        return;
    }

    if (!raycastRenderer)
    {
        raycastRenderer = std::make_unique<CADacomb::Render::SdfRaycastRenderer>(renderer.getDevice());
        dirty           = true;
    }

    const CADacomb::Formula& formula        = asset->getFormula();
    bool                     formulaChanged = (formula.getName() != lastFormulaName) || (formula.getHelper() != lastFormulaHelper) || (formula.getExpression() != lastFormulaExpression);

    if (dirty || formulaChanged)
    {
        std::string error;
        if (!raycastRenderer->setSdf(asset->getSdf(), error) && !error.empty())
        {
            std::cerr << "SdfAssetRenderer shader error: " << error << std::endl;
        }
        lastFormulaName       = formula.getName();
        lastFormulaHelper     = formula.getHelper();
        lastFormulaExpression = formula.getExpression();
        dirty                 = false;
    }

    if (!raycastRenderer->isReady())
    {
        return;
    }

    Polyglot::Node sdfPrefRes = gatedDatabase.execute({
      {    "type", "GetSdfPreferences" },
      { "version",                   1 }
    });
    SdfPreferences sdfPrefs;
    if (sdfPrefRes.contains("preferences"))
    {
        sdfPrefs.fromNode(sdfPrefRes["preferences"]);
    }

    glm::vec3 baseColor(0.72f, 0.78f, 0.86f);
    raycastRenderer->render(renderer.getColor(),
                            renderer.getNormal(),
                            renderer.getDepth(),
                            sceneData,
                            asset->getAABB(),
                            asset->getTransformation(),
                            baseColor,
                            static_cast<uint32_t>(sdfPrefs.getMaxSteps()),
                            static_cast<float>(sdfPrefs.getHitEpsilon()),
                            static_cast<float>(sdfPrefs.getStepScale()),
                            static_cast<float>(sdfPrefs.getNormalEpsilon()),
                            static_cast<float>(sdfPrefs.getAmbientStrength()));
}
