#include "Database/Preferences/SVOPreferences.h"

SVOPreferences::SVOPreferences() {}

SVOPreferences::~SVOPreferences() {}

nlohmann::json SVOPreferences::toJson() const
{
    nlohmann::json jsonObject;
    jsonObject["strategyType"]          = strategyType;
    jsonObject["chunkSize"]             = chunkSize;
    jsonObject["vertexCapacity"]        = vertexCapacity;
    jsonObject["minPixelSize"]          = minPixelSize;
    jsonObject["maxPixelSize"]          = maxPixelSize;
    jsonObject["targetFillRatio"]       = targetFillRatio;
    jsonObject["maxDepth"]              = maxDepth;
    jsonObject["frustumCullingEnabled"] = frustumCullingEnabled;
    jsonObject["nearDistance"]          = nearDistance;
    jsonObject["farDistance"]           = farDistance;
    jsonObject["fixedDecision"]         = fixedDecision;
    return jsonObject;
}

void SVOPreferences::fromJson(const nlohmann::json& jsonObject)
{
    strategyType          = jsonObject.at("strategyType").get<int>();
    chunkSize             = jsonObject.at("chunkSize").get<int>();
    vertexCapacity        = jsonObject.at("vertexCapacity").get<int>();
    minPixelSize          = jsonObject.at("minPixelSize").get<double>();
    maxPixelSize          = jsonObject.at("maxPixelSize").get<double>();
    targetFillRatio       = jsonObject.at("targetFillRatio").get<double>();
    maxDepth              = jsonObject.at("maxDepth").get<int>();
    frustumCullingEnabled = jsonObject.at("frustumCullingEnabled").get<bool>();
    nearDistance          = jsonObject.at("nearDistance").get<double>();
    farDistance           = jsonObject.at("farDistance").get<double>();
    fixedDecision         = jsonObject.at("fixedDecision").get<int>();
    dirty                 = true;
}

int SVOPreferences::getStrategyType() const
{
    return strategyType;
}

void SVOPreferences::setStrategyType(int type)
{
    strategyType = type;
    dirty        = true;
}

int SVOPreferences::getChunkSize() const
{
    return chunkSize;
}

void SVOPreferences::setChunkSize(int size)
{
    chunkSize = size;
    dirty     = true;
}

int SVOPreferences::getVertexCapacity() const
{
    return vertexCapacity;
}

void SVOPreferences::setVertexCapacity(int capacity)
{
    vertexCapacity = capacity;
    dirty          = true;
}

double SVOPreferences::getMinPixelSize() const
{
    return minPixelSize;
}

void SVOPreferences::setMinPixelSize(double size)
{
    minPixelSize = size;
    dirty        = true;
}

double SVOPreferences::getMaxPixelSize() const
{
    return maxPixelSize;
}

void SVOPreferences::setMaxPixelSize(double size)
{
    maxPixelSize = size;
    dirty        = true;
}

double SVOPreferences::getTargetFillRatio() const
{
    return targetFillRatio;
}

void SVOPreferences::setTargetFillRatio(double ratio)
{
    targetFillRatio = ratio;
    dirty           = true;
}

int SVOPreferences::getMaxDepth() const
{
    return maxDepth;
}

void SVOPreferences::setMaxDepth(int depth)
{
    maxDepth = depth;
    dirty    = true;
}

bool SVOPreferences::isFrustumCullingEnabled() const
{
    return frustumCullingEnabled;
}

void SVOPreferences::setFrustumCullingEnabled(bool enabled)
{
    frustumCullingEnabled = enabled;
    dirty                 = true;
}

double SVOPreferences::getNearDistance() const
{
    return nearDistance;
}

void SVOPreferences::setNearDistance(double distance)
{
    nearDistance = distance;
    dirty        = true;
}

double SVOPreferences::getFarDistance() const
{
    return farDistance;
}

void SVOPreferences::setFarDistance(double distance)
{
    farDistance = distance;
    dirty       = true;
}

int SVOPreferences::getFixedDecision() const
{
    return fixedDecision;
}

void SVOPreferences::setFixedDecision(int decision)
{
    fixedDecision = decision;
    dirty         = true;
}

bool SVOPreferences::isRebuildRequested() const
{
    return rebuildRequested;
}

void SVOPreferences::requestRebuild()
{
    rebuildRequested = true;
    dirty            = true;
}

void SVOPreferences::clearRebuildRequested()
{
    rebuildRequested = false;
}

bool SVOPreferences::isDirty() const
{
    return dirty;
}

void SVOPreferences::markClean()
{
    dirty = false;
}

void SVOPreferences::markDirty()
{
    dirty = true;
}
