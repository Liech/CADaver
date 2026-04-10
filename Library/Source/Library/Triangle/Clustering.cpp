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

    //void Clustering::removeHolesByDivision(const Triangulation& tri, std::vector<std::vector<size_t>>& clusters)
    //{
    //    for (size_t clusterId = 0; clusterId < clusters.size(); clusterId++)
    //    {
    //        auto& cluster = clusters[clusterId];
    //        //while (true)
    //        //{
    //        //    auto loops = findLoops(tri, cluster);
    //        //    if (loops.size() == 1)
    //        //        break;
    //        //
    //        //    auto loopDirs = getLoopDirections(tri, loops, cluster);
    //        //    for (size_t i = 0; i < loopDirs.size(); i++)
    //        //    {
    //        //      if (!loopDirs[i]) // Hole!
    //        //      {
    //        //        size_t seedA = 
    //        //        }
    //        //    }
    //        //}
    //    }
    //}

    std::vector<std::vector<size_t>> Clustering::findLoops(const Triangulation& tri, const std::vector<size_t>& trianglesInCluster)
    {
        std::vector<std::vector<size_t>> result;
        auto                             adjacency = tri.getAdjacency();
        if (trianglesInCluster.empty())
            return result;

        const std::vector<size_t>&         indices = tri.indices;
        std::unordered_set<size_t>         clusterSet(trianglesInCluster.begin(), trianglesInCluster.end());
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

        return allLoops;
    }

    std::vector<bool> Clustering::getLoopDirections(const Triangulation& tri, const std::vector<std::vector<size_t>>& allLoops, const std::vector<size_t>& trianglesInCluster)
    {
        const std::vector<glm::dvec3>&                           vertices  = tri.vertices;
        const std::vector<size_t>&                               indices   = tri.indices;
        std::map<std::pair<size_t, size_t>, std::vector<size_t>> adjacency = tri.getAdjacency();

        std::vector<bool> result;
        result.resize(allLoops.size());
        for (size_t i = 0; i < allLoops.size(); i++)
        {
            result[i]        = false;
            const auto& loop = allLoops[i];
            size_t      x1   = loop[0];
            size_t      x2   = loop[0];

            for (const auto& current : trianglesInCluster)
            {
                size_t a = tri.indices[current * 3 + 0];
                size_t b = tri.indices[current * 3 + 1];
                size_t c = tri.indices[current * 3 + 2];

                bool outerloop = (x1 == a && x2 == b) || (x1 == b && x2 == c) || (x1 == c && x2 == a);
                if (outerloop)
                {
                    result[i] = true;
                    break;
                }
            }
        }
        return result;
    }
}