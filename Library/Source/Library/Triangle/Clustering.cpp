#include "Clustering.h"

#include "Triangulation.h"
#include <queue>
#include <unordered_set>

namespace Library
{
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

    std::vector<std::vector<size_t>> Clustering::findHoles(const Triangulation& tri, const std::vector<size_t>& trianglesInCluster)
    {
        std::vector<std::vector<size_t>> result;
        auto                             adjacency = tri.getAdjacency();
        if (trianglesInCluster.empty())
            return result;

        const std::vector<size_t>& indices = tri.indices;
        std::unordered_set<size_t> clusterSet(trianglesInCluster.begin(), trianglesInCluster.end());

        // 1. Collect all boundary edges (edges where the "other side" isn't in the cluster)
        // Map: start_node -> end_node
        std::unordered_map<size_t, size_t> nextVertex;

        for (size_t triIdx : trianglesInCluster)
        {
            for (int i = 0; i < 3; ++i)
            {
                size_t v1 = indices[triIdx * 3 + i];
                size_t v2 = indices[triIdx * 3 + (i + 1) % 3];

                auto        edgeKey   = (v1 < v2) ? std::make_pair(v1, v2) : std::make_pair(v2, v1);
                const auto& neighbors = adjacency.at(edgeKey);

                bool isBoundary = true;
                for (size_t n : neighbors)
                {
                    if (n != triIdx && clusterSet.count(n))
                    {
                        isBoundary = false; // It's an internal edge within the cluster
                        break;
                    }
                }

                if (isBoundary)
                {
                    nextVertex[v1] = v2;
                }
            }
        }

        // 2. Trace loops
        std::unordered_set<size_t>       visited;
        std::vector<std::vector<size_t>> allLoops;

        for (auto const& [startNode, _] : nextVertex)
        {
            if (visited.count(startNode))
                continue;

            std::vector<size_t> loop;
            size_t              curr = startNode;
            while (nextVertex.count(curr) && !visited.count(curr))
            {
                visited.insert(curr);
                loop.push_back(curr);
                curr = nextVertex[curr];
            }
            if (loop.size() >= 3)
                allLoops.push_back(loop);
        }

        // 3. Distinguish Holes from Perimeter
        // In a 2D-manifold cluster, the perimeter is the loop that "encloses"
        // the triangles. A hole is a loop that is "enclosed by" the triangles.
        // The simplest topological check: The perimeter winds CCW, holes wind CW
        // (relative to the surface).

        // Since we can't use "Area" without coordinates, we use the property that
        // the outer boundary is the one that contains all others.
        // If you have coordinates, the "largest absolute area" is the perimeter.
        // If you truly want NO coordinates, you'd need the full mesh boundary.

        return allLoops;
    }
}