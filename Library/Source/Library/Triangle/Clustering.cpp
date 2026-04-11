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

    bool Clustering::isHole(const HalfEdgeMesh& mesh, const std::vector<size_t>& vertexLoop, const std::unordered_set<size_t>& clusterFaces)
    {
        if (vertexLoop.size() < 3)
            return false;

        // 1. Find the half-edge connecting vertexLoop[0] -> vertexLoop[1]
        int64_t vStart     = vertexLoop[0];
        int64_t vNext      = vertexLoop[1];
        int64_t borderEdge = SafeNull;

        // Start at the vertex and rotate through outgoing edges
        int64_t curr  = mesh.vertices[vStart].half_edge;
        int64_t start = curr;
        if (curr != SafeNull)
        {
            do
            {
                if (mesh.half_edges[curr].target_vertex == vNext)
                {
                    // We found the edge. Now check if this edge is a cluster boundary.
                    int64_t twin   = mesh.half_edges[curr].twin;
                    bool    faceIn = clusterFaces.contains(mesh.half_edges[curr].face);
                    bool    twinIn = (twin != SafeNull) ? clusterFaces.contains(mesh.half_edges[twin].face) : false;

                    if (faceIn != twinIn)
                    {
                        borderEdge = curr;
                        break;
                    }
                }
                // Pivot to next outgoing edge around vStart
                int64_t twin = mesh.half_edges[curr].twin;
                if (twin == SafeNull)
                    break;
                curr = mesh.half_edges[mesh.half_edges[twin].next].next; // Pivot logic
            } while (curr != start && curr != SafeNull);
        }

        if (borderEdge == SafeNull)
            return false;

        // 2. Logic: If the face of the edge going v0->v1 is NOT in the cluster,
        // it means the cluster is to the "right" of the path, signifying a hole.
        bool leftFaceInCluster = clusterFaces.contains(mesh.half_edges[borderEdge].face);

        return !leftFaceInCluster;
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
                bool pushedToTodo = false;
                auto clusterSet   = std::unordered_set<size_t>(cluster.begin(), cluster.end());
                for (const auto& border : borders)
                {
                    if (isHole(*halfedge, border, clusterSet))
                    {
                        size_t seedA       = border[0];
                        size_t seedB       = border[(border.size() - 1) / 2];
                        auto   newClusters = splitCluster(tri, cluster, { seedA, seedB });
                        for (const auto& x : newClusters)
                            todo.push(x);
                        pushedToTodo = true;
                        break;
                    }
                }
                if (!pushedToTodo) // a surface of an hole e.g. could have multiple borders and no hole
                {
                    result.push_back(cluster);
                }
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

TEST_CASE("Clustering::findBorders with Holed Surface", "[clustering][topology]")
{
    using namespace Library;

    // 1. Setup
    Triangulation tri  = CreateHoledSquare();
    auto          mesh = mesh2halfedge::convert(tri);

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

    SECTION("Hole detection logic")
    {
        std::unordered_set<size_t> clusterSet(cluster.begin(), cluster.end());

        // Sort borders so index 1 is the smaller loop (the hole)
        std::sort(borders.begin(), borders.end(), [](const auto& a, const auto& b) { return a.size() > b.size(); });

        // isHole should be false for the outer boundary
        CHECK(clustering.isHole(*mesh, borders[0], clusterSet) == false);

        // isHole should be true for the inner boundary
        CHECK(clustering.isHole(*mesh, borders[1], clusterSet) == true);
    }
}
#endif