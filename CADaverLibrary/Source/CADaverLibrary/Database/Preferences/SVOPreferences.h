#pragma once

#include <nlohmann/json.hpp>
#include <string>

// Holds settings for SVO LOD meshing and visualization strategies.
class SVOPreferences
{
  public:
    SVOPreferences();
    virtual ~SVOPreferences();

    nlohmann::json toJson() const;
    void           fromJson(const nlohmann::json& jsonObject);

    int  getStrategyType() const;
    void setStrategyType(int type);

    int  getChunkSize() const;
    void setChunkSize(int size);

    int  getVertexCapacity() const;
    void setVertexCapacity(int capacity);

    double getMinPixelSize() const;
    void   setMinPixelSize(double size);

    double getMaxPixelSize() const;
    void   setMaxPixelSize(double size);

    double getTargetFillRatio() const;
    void   setTargetFillRatio(double ratio);

    int  getMaxDepth() const;
    void setMaxDepth(int depth);

    bool isFrustumCullingEnabled() const;
    void setFrustumCullingEnabled(bool enabled);

    double getNearDistance() const;
    void   setNearDistance(double distance);

    double getFarDistance() const;
    void   setFarDistance(double distance);

    int  getFixedDecision() const;
    void setFixedDecision(int decision);

    bool isRebuildRequested() const;
    void requestRebuild();
    void clearRebuildRequested();

    bool isDirty() const;
    void markClean();
    void markDirty();

  private:
    double minPixelSize          = 1.0;
    double maxPixelSize          = 4.0;
    double targetFillRatio       = 0.85;
    double nearDistance          = 10.0;
    double farDistance           = 100.0;
    int    strategyType          = 0;
    int    chunkSize             = 32;
    int    vertexCapacity        = 500000;
    int    maxDepth              = -1;
    int    fixedDecision         = 1;
    bool   frustumCullingEnabled = true;
    bool   rebuildRequested      = false;
    bool   dirty                 = true;
};
