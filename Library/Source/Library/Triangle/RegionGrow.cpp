#include "RegionGrow.h"

#include "Triangulation.h"
#include <queue>

namespace Library
{
    std::vector<std::vector<size_t>> RegionGrow::grow(const Triangulation& tri, std::function<bool(size_t currentIndex, size_t candidateIndex, const Triangulation&)> growFunction)
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
}