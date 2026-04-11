#include "Clustering.h"

#include "HalfEdge/HalfEdge.h"
#include "HalfEdge/mesh2halfedge.h"
#include "Triangulation.h"
#include <queue>
#include <unordered_set>
#include <algorithm>

namespace Library
{
    Clustering::Clustering() {}
    Clustering::~Clustering() {}

    std::vector<std::vector<size_t>> Clustering::cluster(const Triangulation& tri, std::function<bool(size_t currentIndex, size_t candidateIndex, const Triangulation&)> growFunction)
    {
        size_t                                                   numTriangles = tri.indices.size() / 3;
        std::vector<std::vector<size_t>>                         result;
        std::map<std::pair<size_t, size_t>, std::vector<size_t>> adjacency = tri.getAdjacency();
        std::vector<bool>                                        visited(numTriangles, false);
        for (size_t i = 0; i < numTriangles; ++i)
        {
            if (visited[i])
                continue;

            // Start a new patch (Region)
            std::vector<size_t> currentPatch;
            std::queue<size_t>  processQueue;

            processQueue.push(i);
            visited[i] = true;

            while (!processQueue.empty())
            {
                size_t currIdx = processQueue.front();
                processQueue.pop();
                currentPatch.push_back(currIdx);

                // 2. Identify neighbors through shared edges
                // Triangle currIdx has vertices at indices[currIdx*3 + 0, 1, 2]
                size_t v[3] = { tri.indices[currIdx * 3 + 0], tri.indices[currIdx * 3 + 1], tri.indices[currIdx * 3 + 2] };

                // Define the three edges of the current triangle
                std::pair<size_t, size_t> edges[3] = {
                    { v[0], v[1] },
                    { v[1], v[2] },
                    { v[2], v[0] }
                };

                for (auto& edge : edges)
                {
                    // Sort edge indices to match the adjacency map key format
                    auto key = (edge.first < edge.second) ? std::make_pair(edge.first, edge.second) : std::make_pair(edge.second, edge.first);

                    if (adjacency.count(key))
                    {
                        for (size_t neighborIdx : adjacency[key])
                        {
                            if (!visited[neighborIdx])
                            {
                                // 3. Use the abstraction to decide if we should grow
                                if (growFunction(currIdx, neighborIdx, tri))
                                {
                                    visited[neighborIdx] = true;
                                    processQueue.push(neighborIdx);
                                }
                            }
                        }
                    }
                }
            }
            result.push_back(currentPatch);
        }

        return result;
    }

    std::vector<std::vector<size_t>> Clustering::findBorders(const HalfEdgeMesh& mesh, const std::vector<size_t>& trianglesInCluster)
    {
        std::unordered_set<int64_t> clusterFaces(trianglesInCluster.begin(), trianglesInCluster.end());

        // Map: Source Vertex -> HalfEdge index
        // This allows us to instantly find which boundary edge starts where another ends.
        std::unordered_map<size_t, int64_t> vertexToEdge;
        std::vector<int64_t>                boundaryEdges;

        // 1. Identify all perimeter edges
        for (size_t fIdx : trianglesInCluster)
        {
            int64_t startEdge = mesh.faces[fIdx].half_edge;
            int64_t curr      = startEdge;
            do
            {
                int64_t twinEdge = mesh.half_edges[curr].twin;
                int64_t twinFace = (twinEdge != SafeNull) ? mesh.half_edges[twinEdge].face : SafeNull;

                // If twinFace is outside the cluster or is a physical hole (SafeNull)
                if (clusterFaces.find(twinFace) == clusterFaces.end())
                {
                    size_t src        = static_cast<size_t>(mesh.source(curr));
                    vertexToEdge[src] = curr;
                    boundaryEdges.push_back(curr);
                }
                curr = mesh.half_edges[curr].next;
            } while (curr != startEdge);
        }

        std::vector<std::vector<size_t>> allLoops;
        std::unordered_set<int64_t>      visited;

        // 2. Trace loops using the vertex-to-edge map
        for (int64_t edge : boundaryEdges)
        {
            if (visited.count(edge))
                continue;

            std::vector<size_t> currentLoop;
            int64_t             curr = edge;

            while (visited.find(curr) == visited.end())
            {
                visited.insert(curr);

                int64_t srcVtx = mesh.source(curr);
                int64_t dstVtx = mesh.source(mesh.half_edges[curr].next);
                currentLoop.push_back(srcVtx);

                // The next edge in the loop MUST start at the current edge's destination
                auto it = vertexToEdge.find(dstVtx);
                if (it == vertexToEdge.end())
                {
                    // This happens if the mesh is non-manifold at this vertex
                    break;
                }

                curr = it->second;
                if (curr == edge)
                    break;
            }

            if (!currentLoop.empty())
            {
                allLoops.push_back(currentLoop);
            }
        }

        return allLoops;
    }

    std::vector<std::vector<size_t>> Clustering::splitCluster(const Triangulation& tri, const std::vector<size_t>& cluster, const std::vector<size_t>& looptoBreak)
    {
        if (looptoBreak.empty())
            return { cluster };

        std::unordered_set<size_t> clusterSet(cluster.begin(), cluster.end());
        std::set<size_t>           startSeedCandidates;
        std::unordered_set<size_t> loopVertices(looptoBreak.begin(), looptoBreak.end());

        for (size_t face : cluster)
        {
            for (int i = 0; i < 3; ++i)
            {
                size_t id = tri.indices[face * 3 + i];
                if (loopVertices.contains(id))
                {
                    startSeedCandidates.insert(face);
                }
            }
        }

        if (startSeedCandidates.size() < 2)
            return { cluster };

        auto                it     = startSeedCandidates.begin();
        size_t              first  = *it;
        size_t              second = *(++it);
        std::vector<size_t> seeds  = { first, second };

        std::map<std::pair<size_t, size_t>, std::vector<size_t>> adjacency = tri.getAdjacency();

        // Track which seed owns which triangle: {TriangleIndex -> SeedIndex}
        std::unordered_map<size_t, size_t> ownership;
        std::queue<size_t>                 q;

        // Initialize
        for (size_t s : seeds)
        {
            ownership[s] = s;
            q.push(s);
        }

        // 2. Growth Loop
        while (!q.empty())
        {
            size_t currentTri = q.front();
            q.pop();
            size_t currentOwner = ownership[currentTri];

            // Find neighbors of currentTri.
            // In a mesh, currentTri has 3 edges. We check the map for each.
            size_t baseIdx     = currentTri * 3;
            size_t triVerts[3] = { tri.indices[baseIdx], tri.indices[baseIdx + 1], tri.indices[baseIdx + 2] };

            for (int i = 0; i < 3; ++i)
            {
                size_t v1 = triVerts[i];
                size_t v2 = triVerts[(i + 1) % 3];
                if (v1 > v2)
                    std::swap(v1, v2);

                // Get triangles sharing this edge
                const auto& neighbors = adjacency.at({ v1, v2 });
                for (size_t neighborTri : neighbors)
                {
                    // Growth Rules:
                    // - Must be part of the original cluster
                    // - Must not be the current triangle itself
                    // - Must not be claimed by another seed yet
                    if (neighborTri != currentTri && clusterSet.count(neighborTri) && ownership.find(neighborTri) == ownership.end())
                    {

                        ownership[neighborTri] = currentOwner;
                        q.push(neighborTri);
                    }
                }
            }
        }

        // 3. Collect Results
        std::vector<size_t> clusterA, clusterB;
        for (auto const& [triIdx, owner] : ownership)
        {
            if (owner == seeds[0])
                clusterA.push_back(triIdx);
            else
                clusterB.push_back(triIdx);
        }

        std::set<size_t> clusterASet(clusterA.begin(),clusterA.end());
        for (const auto& x : clusterB)
            assert(!clusterASet.contains(x));

        return { clusterA, clusterB };
    }

    std::vector<std::vector<size_t>> Clustering::cluster_withoutHoles(const Triangulation& tri, std::function<bool(size_t currentIndex, size_t candidateIndex, const Triangulation&)> growFunction)
    {
        std::vector<std::vector<size_t>> clusters = cluster(tri, growFunction);
        std::vector<std::vector<size_t>> result;
        auto                             halfedge = mesh2halfedge::convert(tri);
        std::queue<std::vector<size_t>>  todo;
        for (const auto& x : clusters)
            todo.push(x);

        while (!todo.empty())
        {
            auto cluster = todo.front();
            todo.pop();

            std::vector<std::vector<size_t>> borders = findBorders(*halfedge, cluster);

            if (borders.size() <= 1)
                result.push_back(cluster);
            else
            {
                std::sort(borders.begin(), borders.end(), [](const std::vector<size_t>& a, const std::vector<size_t>& b) { return a.size() < b.size(); });
                const auto& border      = borders[0];
                size_t      seedA       = border[0];
                size_t      seedB       = border[(border.size() - 1) / 2];
                auto        newClusters = splitCluster(tri, cluster, border);
                for (const auto& x : newClusters)
                    todo.push(x);
            }
        }

        return result;
    }
}

#ifdef ISTESTPROJECT
#include "Library/catch.hpp"
using namespace Library;

Triangulation CreateHoledSquare()
{
    Triangulation tri;
    // Create a 4x4 vertex grid (3x3 quads)
    for (int y = 0; y < 4; ++y)
    {
        for (int x = 0; x < 4; ++x)
        {
            tri.vertices.push_back(glm::dvec3(x, y, 0));
        }
    }

    // Helper to get index at (x, y)
    auto idx = [](int x, int y) { return y * 4 + x; };

    for (int y = 0; y < 3; ++y)
    {
        for (int x = 0; x < 3; ++x)
        {
            // Skip the center quad (x=1, y=1) to create a hole
            if (x == 1 && y == 1)
                continue;

            // Triangle 1
            tri.indices.push_back(idx(x, y));
            tri.indices.push_back(idx(x + 1, y));
            tri.indices.push_back(idx(x + 1, y + 1));
            // Triangle 2
            tri.indices.push_back(idx(x, y));
            tri.indices.push_back(idx(x + 1, y + 1));
            tri.indices.push_back(idx(x, y + 1));
        }
    }
    return tri;
}

#include "HalfEdge/HalfEdgeHealth.h"
TEST_CASE("Clustering::findBorders with Holed Surface", "[clustering][topology]")
{
    using namespace Library;

    // 1. Setup
    Triangulation tri       = CreateHoledSquare();
    auto          mesh      = mesh2halfedge::convert(tri);
    auto          report    = HalfEdgeHealth::createReport(*mesh);
    auto          trireport = HalfEdgeHealth::createReport(tri);
    auto          content   = HalfEdgeHealth::toString(*mesh);

    // The cluster is the entire mesh (all faces)
    std::vector<size_t> cluster;
    for (size_t i = 0; i < tri.indices.size() / 3; ++i)
    {
        cluster.push_back(i);
    }

    // 2. Execute
    Clustering                       clustering;
    std::vector<std::vector<size_t>> borders = clustering.findBorders(*mesh, cluster);

    // 3. Assertions
    SECTION("Correct number of loops detected")
    {
        // A square with a hole has 2 boundaries
        REQUIRE(borders.size() == 2);
    }

    SECTION("Loops contain correct vertex counts")
    {
        // Sort by size so we know which is which
        std::sort(borders.begin(), borders.end(), [](const auto& a, const auto& b) { return a.size() > b.size(); });

        // Outer boundary: 4 edges * 3 = 12 vertices (perimeter of 4x4 grid)
        CHECK(borders[0].size() == 12);

        // Inner hole: 4 vertices (perimeter of the deleted center quad)
        CHECK(borders[1].size() == 4);
    }
}
#endif