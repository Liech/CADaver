#pragma once

#include <functional>
#include <set>
#include <vector>
#include <unordered_set>

namespace Library
{
    class Triangulation;
    struct HalfEdgeMesh;

    class Clustering
    {
      public:
        Clustering();
        virtual ~Clustering();

        // Returns vector of regions consisting of triangle inidices
        static std::vector<std::vector<size_t>> cluster(const Triangulation&, std::function<bool(size_t currentIndex, size_t candidateIndex, const Triangulation&)> growFunction);
        static std::vector<std::vector<size_t>> cluster_withoutHoles(const Triangulation&, std::function<bool(size_t currentIndex, size_t candidateIndex, const Triangulation&)> growFunction);

      public:
        static std::vector<std::vector<size_t>> findBorders(const HalfEdgeMesh&, const std::vector<size_t>& trianglesInCluster);
        static bool                             isHole(const HalfEdgeMesh& mesh, const std::vector<size_t>& loopEdges, const std::unordered_set<size_t>& clusterFaces); // is the found border a hole?
        static std::vector<std::vector<size_t>> splitCluster(const Triangulation&, const std::vector<size_t>& TriangleCluster, std::vector<size_t> VertexSeeds);
    };
}