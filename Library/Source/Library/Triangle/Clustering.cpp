#include "Clustering.h"

#include "HalfEdge/HalfEdge.h"
#include "HalfEdge/mesh2halfedge.h"
#include "Triangulation.h"
#include <queue>
#include <unordered_set>

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

    bool Clustering::isHole(const HalfEdgeMesh& mesh, const std::vector<size_t>& loopEdges, const std::unordered_set<size_t>& clusterFaces)
    {
        if (loopEdges.size() < 3)
            return false;
        int64_t e        = loopEdges[0];
        int64_t leftFace = mesh.half_edges[e].face;

        if (clusterFaces.find(leftFace) == clusterFaces.end())
        {
            // debug sanity check
            auto twin = mesh.twin(e);
            if (twin != SafeNull)
            {
                auto twinFace = mesh.half_edges[twin].face;
                assert(clusterFaces.contains(twinFace));
            }

            return true;
        }
        return false;
    }

    std::vector<std::vector<size_t>> Clustering::findBorders(const HalfEdgeMesh& mesh, const std::vector<size_t>& trianglesInCluster)
    {
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

    std::vector<std::vector<size_t>> Clustering::splitCluster(const Triangulation& tri, const std::vector<size_t>& cluster, std::vector<size_t> seeds)
    {
        if (seeds.empty())
            return { cluster };

        // 1. Map: Vertex ID -> List of Face Indices (only for faces in this cluster)
        std::unordered_map<size_t, std::vector<size_t>> vertexToFaces;
        std::unordered_set<size_t>                      clusterSet(cluster.begin(), cluster.end());

        for (size_t faceIdx : cluster)
        {
            for (int i = 0; i < 3; ++i)
            {
                size_t vIdx = tri.indices[faceIdx * 3 + i];
                vertexToFaces[vIdx].push_back(faceIdx);
            }
        }

        // 2. Setup BFS
        // assignments: FaceIndex -> SeedGroupIndex
        std::unordered_map<size_t, size_t> assignments;
        std::queue<size_t>                 traversalQueue;

        for (size_t i = 0; i < seeds.size(); ++i)
        {
            size_t seedVtx = seeds[i];
            if (vertexToFaces.count(seedVtx))
            {
                for (size_t faceIdx : vertexToFaces[seedVtx])
                {
                    if (assignments.find(faceIdx) == assignments.end())
                    {
                        assignments[faceIdx] = i;
                        traversalQueue.push(faceIdx);
                    }
                }
            }
        }

        // 3. Expand via Edge Adjacency
        while (!traversalQueue.empty())
        {
            size_t currentFace = traversalQueue.front();
            traversalQueue.pop();
            size_t currentSeedId = assignments[currentFace];

            // For each of the 3 vertices in the current face
            for (int i = 0; i < 3; ++i)
            {
                size_t vIdx = tri.indices[currentFace * 3 + i];

                // Check all faces connected to this vertex (Neighbor candidates)
                for (size_t neighborFace : vertexToFaces[vIdx])
                {
                    if (assignments.find(neighborFace) == assignments.end())
                    {
                        // To be a strict "neighbor", they should share an edge (2 vertices).
                        // However, in a BFS growth, sharing a vertex is often sufficient
                        // and faster. For manifold meshes, vertex-sharing expansion works well.
                        assignments[neighborFace] = currentSeedId;
                        traversalQueue.push(neighborFace);
                    }
                }
            }
        }

        // 4. Pack results
        std::vector<std::vector<size_t>> result(seeds.size());
        for (const auto& [faceIdx, seedId] : assignments)
        {
            result[seedId].push_back(faceIdx);
        }

        return result;
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

            auto borders = findBorders(*halfedge, cluster);

            if (borders.size() <= 1)
                result.push_back(cluster);
            else
            {
                auto clusterSet = std::unordered_set<size_t>(cluster.begin(), cluster.end());
                for (const auto& border : borders)
                {
                    if (isHole(*halfedge, border, clusterSet))
                    {
                        size_t seedA       = border[0];
                        size_t seedB       = border[(border.size() - 1) / 2];
                        auto   newClusters = splitCluster(tri, cluster, { seedA, seedB });
                        for (const auto& x : newClusters)
                            todo.push(x);
                        break;
                    }
                }
            }
        }

        return result;
    }
}
