#include "SVOPreferencesView.h"

#include "Database/Preferences/SVOPreferences.h"

#include <algorithm>
#include <cstdio>
#include <imgui.h>

SVOPreferencesView::SVOPreferencesView() {}

SVOPreferencesView::~SVOPreferencesView() {}

bool SVOPreferencesView::render(SVOPreferences& preferences)
{
    bool modified = false;

    ImGui::Text("SVO Meshing & Capacity");
    ImGui::Separator();
    ImGui::Spacing();

    static const int chunkValues[3]  = { 16, 32, 64 };
    int              currentChunk    = preferences.getChunkSize();
    int              currentChunkIdx = (currentChunk == 16) ? 0 : ((currentChunk == 64) ? 2 : 1);
    if (ImGui::Combo("Chunk Size", &currentChunkIdx, "16\00032\00064\000\000"))
    {
        preferences.setChunkSize(chunkValues[currentChunkIdx]);
        modified = true;
    }

    int vertexCapacity = preferences.getVertexCapacity();
    if (ImGui::InputInt("Vertex Capacity", &vertexCapacity, 50000, 100000))
    {
        preferences.setVertexCapacity(std::max(10000, vertexCapacity));
        modified = true;
    }

    if (ImGui::Button("Rebuild Mesh"))
    {
        preferences.requestRebuild();
        modified = true;
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Text("LOD Strategy");
    ImGui::Separator();
    ImGui::Spacing();

    static const char* strategyNames[] = { "Frustum Adaptive", "Screen Space", "Capacity Target", "Distance Band", "Fixed" };
    int                strategyType    = preferences.getStrategyType();
    if (ImGui::Combo("Strategy", &strategyType, strategyNames, 5))
    {
        preferences.setStrategyType(strategyType);
        modified = true;
    }

    if (strategyType == 0)
    {
        float minPix = static_cast<float>(preferences.getMinPixelSize());
        float maxPix = static_cast<float>(preferences.getMaxPixelSize());
        if (ImGui::SliderFloat("Coarsen Pixels (Min)", &minPix, 0.5f, 10.0f, "%.1f px"))
        {
            preferences.setMinPixelSize(static_cast<double>(minPix));
            modified = true;
        }
        if (ImGui::SliderFloat("Refine Pixels (Max)", &maxPix, 1.0f, 30.0f, "%.1f px"))
        {
            preferences.setMaxPixelSize(static_cast<double>(maxPix));
            modified = true;
        }

        float targetFill = static_cast<float>(preferences.getTargetFillRatio());
        if (ImGui::SliderFloat("Target Fill Ratio", &targetFill, 0.5f, 0.99f, "%.2f"))
        {
            preferences.setTargetFillRatio(static_cast<double>(targetFill));
            modified = true;
        }

        int  maxDepth = preferences.getMaxDepth();
        char depthDisplay[32];
        if (maxDepth < 0)
        {
            snprintf(depthDisplay, sizeof(depthDisplay), "Unlimited");
        }
        else
        {
            snprintf(depthDisplay, sizeof(depthDisplay), "%d", maxDepth);
        }
        if (ImGui::SliderInt("Max Tree Depth", &maxDepth, -1, 16, depthDisplay))
        {
            preferences.setMaxDepth(maxDepth);
            modified = true;
        }

        bool culling = preferences.isFrustumCullingEnabled();
        if (ImGui::Checkbox("Frustum Culling", &culling))
        {
            preferences.setFrustumCullingEnabled(culling);
            modified = true;
        }
    }
    else if (strategyType == 1)
    {
        float minPix = static_cast<float>(preferences.getMinPixelSize());
        float maxPix = static_cast<float>(preferences.getMaxPixelSize());
        if (ImGui::SliderFloat("Coarsen Pixels (Min)", &minPix, 0.5f, 10.0f, "%.1f px"))
        {
            preferences.setMinPixelSize(static_cast<double>(minPix));
            modified = true;
        }
        if (ImGui::SliderFloat("Refine Pixels (Max)", &maxPix, 1.0f, 30.0f, "%.1f px"))
        {
            preferences.setMaxPixelSize(static_cast<double>(maxPix));
            modified = true;
        }
    }
    else if (strategyType == 2)
    {
        float targetFill = static_cast<float>(preferences.getTargetFillRatio());
        if (ImGui::SliderFloat("Target Fill Ratio", &targetFill, 0.5f, 0.99f, "%.2f"))
        {
            preferences.setTargetFillRatio(static_cast<double>(targetFill));
            modified = true;
        }
    }
    else if (strategyType == 3)
    {
        float nearDist = static_cast<float>(preferences.getNearDistance());
        float farDist  = static_cast<float>(preferences.getFarDistance());
        if (ImGui::SliderFloat("Near Distance", &nearDist, 1.0f, 100.0f, "%.1f m"))
        {
            preferences.setNearDistance(static_cast<double>(nearDist));
            modified = true;
        }
        if (ImGui::SliderFloat("Far Distance", &farDist, 10.0f, 500.0f, "%.1f m"))
        {
            preferences.setFarDistance(static_cast<double>(farDist));
            modified = true;
        }
    }
    else
    {
        int fixedDecision = preferences.getFixedDecision();
        if (ImGui::RadioButton("Refine", fixedDecision == 0))
        {
            preferences.setFixedDecision(0);
            modified = true;
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("Keep", fixedDecision == 1))
        {
            preferences.setFixedDecision(1);
            modified = true;
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("Coarsen", fixedDecision == 2))
        {
            preferences.setFixedDecision(2);
            modified = true;
        }
    }

    return modified;
}
