#include "Search/AssetSearchProvider.h"

#include "Database/Asset.h"
#include "Database/Asset/CadAsset.h"
#include "Database/Asset/ProgressBarAsset.h"
#include "Database/Asset/TriangulationAsset.h"
#include "Database/Asset/VoxelAsset.h"
#include "Database/Component/TextNoteComponent.h"
#include "Database/Project.h"
#include "Execution/GatedDatabase.h"
#include "Windows/Windows.h"

AssetSearchProvider::AssetSearchProvider(GatedDatabase& gatedDatabase, Windows& windows)
  : gatedDatabase(gatedDatabase)
  , windows(windows)
{
}

AssetSearchProvider::~AssetSearchProvider() {}

std::vector<SearchResult> AssetSearchProvider::search(const std::string& query)
{
    std::vector<SearchResult> results;
    if (query.empty())
    {
        return results;
    }

    const auto& project = gatedDatabase.get().getProject();
    const auto& assets  = project.getAssets();

    for (const auto& asset : assets)
    {
        if (!asset)
        {
            continue;
        }

        std::string summary    = buildAssetSummary(asset);
        bool        assetMatch = matcher.matchesAny(query, asset->getName() + " " + asset->getType(), summary);

        if (assetMatch)
        {
            std::string title = asset->getName() + " (" + asset->getType() + ")";
            results.emplace_back(title, "Asset", summary, [this]() { windows.open("ProjectWindow"); });
        }

        auto notes = asset->getAttachments<TextNoteComponent>();
        for (const auto& note : notes)
        {
            if (!note)
            {
                continue;
            }

            if (matcher.matchesAny(query, note->getName(), note->getText()))
            {
                std::string noteTitle = asset->getName() + " -> Note: " + note->getName();
                std::string noteDesc  = "Asset: " + asset->getName() + " (" + asset->getType() + ")\nNote: " + note->getName() + "\nContent: " + note->getText();
                results.emplace_back(noteTitle, "Note", noteDesc, [this]() { windows.open("ProjectWindow"); });
            }
        }
    }

    return results;
}

std::string AssetSearchProvider::buildAssetSummary(const std::shared_ptr<const Asset>& asset) const
{
    std::string summary = "Name: " + asset->getName() + "\nType: " + asset->getType() + "\nRevision: " + std::to_string(asset->getRevision());

    auto voxel = std::dynamic_pointer_cast<const VoxelAsset>(asset);
    if (voxel)
    {
        uint32_t dim = voxel->getSVO().getDimension();
        summary += "\nResolution: " + std::to_string(dim) + "x" + std::to_string(dim) + "x" + std::to_string(dim);
    }

    auto tri = std::dynamic_pointer_cast<const TriangulationAsset>(asset);
    if (tri)
    {
        const auto& triangulation = tri->getTriangulation();
        size_t      triCount      = triangulation.indices.empty() ? (triangulation.vertices.size() / 3) : (triangulation.indices.size() / 3);
        summary += "\nTriangles: " + std::to_string(triCount);
    }

    auto cad = std::dynamic_pointer_cast<const CadAsset>(asset);
    if (cad)
    {
        const auto& triangulation = cad->getTriangulation();
        size_t      triCount      = triangulation.indices.empty() ? (triangulation.vertices.size() / 3) : (triangulation.indices.size() / 3);
        summary += "\nTriangles: " + std::to_string(triCount);
    }

    auto progress = std::dynamic_pointer_cast<const ProgressBarAsset>(asset);
    if (progress)
    {
        int pct = static_cast<int>(progress->getProgress() * 100.0f);
        summary += "\nTask: " + progress->getOperationName() + " (" + std::to_string(pct) + "%)";
    }

    auto notes = asset->getAttachments<TextNoteComponent>();
    if (!notes.empty())
    {
        summary += "\nNotes (" + std::to_string(notes.size()) + "):";
        for (const auto& note : notes)
        {
            if (note)
            {
                summary += "\n - " + note->getName() + ": " + note->getText();
            }
        }
    }

    return summary;
}

#ifdef ISTESTPROJECT
#include "Database/Asset/VoxelAsset.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("AssetSearchProvider/SearchEmpty")
{
    GatedDatabase       gatedDatabase;
    Windows             windows;
    AssetSearchProvider provider(gatedDatabase, windows);

    auto results = provider.search("NonExistent");
    REQUIRE(results.empty());
}
#endif
