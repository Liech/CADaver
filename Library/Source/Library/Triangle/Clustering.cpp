#include "Clustering.h"

#include "HalfEdge/HalfEdge.h"
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

    bool isHole(const HalfEdgeMesh& mesh, const std::vector<int64_t>& loopEdges, const std::unordered_set<int64_t>& clusterFaces)
    {
        if (loopEdges.empty())
            return false;

        int64_t e = loopEdges[0];
        int64_t t = mesh.half_edges[e].twin;
        if (clusterFaces.count(mesh.half_edges[e].face))
        {
            return false;
        }
        return true;
    }

    std::vector<std::vector<size_t>> Clustering::findBorders(const HalfEdgeMesh& mesh, const std::vector<size_t>& trianglesInCluster) {
        std::unordered_set<int64_t> clusterFaces(trianglesInCluster.begin(), trianglesInCluster.end());
        std::unordered_set<int64_t> boundaryEdges;

        // 1. Collect all half-edges that sit on the perimeter of the cluster
        for (size_t fIdx : trianglesInCluster)
        {
            int64_t startEdge = mesh.faces[fIdx].half_edge;
            int64_t curr      = startEdge;
            do
            {
                int64_t twinEdge = mesh.half_edges[curr].twin;
                int64_t twinFace = (twinEdge != SafeNull) ? mesh.half_edges[twinEdge].face : SafeNull;

                // If the twin is outside the cluster (or doesn't exist), this is a boundary
                if (clusterFaces.find(twinFace) == clusterFaces.end())
                {
                    boundaryEdges.insert(curr);
                }
                curr = mesh.half_edges[curr].next;
            } while (curr != startEdge);
        }

        std::vector<std::vector<size_t>> allLoops;
        std::unordered_set<int64_t>      visited;

        // 2. Trace loops
        for (int64_t edge : boundaryEdges)
        {
            if (visited.count(edge))
                continue;

            std::vector<size_t> currentLoop;
            int64_t             curr = edge;

            while (visited.find(curr) == visited.end())
            {
                visited.insert(curr);
                // Add the source vertex of the boundary half-edge to the loop
                currentLoop.push_back(static_cast<size_t>(mesh.source(curr)));

                // Find the next boundary edge in the loop.
                // We look at the target of curr, and check outgoing edges
                // until we find one that is also a boundary edge.
                int64_t nextPotential = mesh.half_edges[curr].next;

                // If the next edge in the face is already a boundary, easy.
                // Otherwise, we must pivot around the vertex (target_vertex)
                // to find the next boundary edge connected to this vertex.
                while (boundaryEdges.find(nextPotential) == boundaryEdges.end())
                {
                    nextPotential = mesh.half_edges[mesh.half_edges[nextPotential].twin].next;
                }

                curr = nextPotential;
                if (curr == edge)
                    break;
            }
            allLoops.push_back(currentLoop);
        }

        return allLoops;
    }
}
