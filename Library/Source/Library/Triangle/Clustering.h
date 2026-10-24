#pragma once

#include <functional>
#include <set>
#include <unordered_set>
#include <vector>

namespace CADacomb
{
    class Triangulation;
    class HalfEdgeMesh;
}

namespace Library
{
    class Clustering
    {
      public:
        Clustering();
        virtual ~Clustering();

        // Returns vector of regions consisting of triangle inidices
        static std::vector<std::vector<size_t>> cluster(const CADacomb::Triangulation&, std::function<bool(size_t currentIndex, size_t candidateIndex, const CADacomb::Triangulation&)> growFunction);
        static std::vector<std::vector<size_t>> cluster_withoutHoles(const CADacomb::Triangulation&,
                                                                     std::function<bool(size_t currentIndex, size_t candidateIndex, const CADacomb::Triangulation&)> growFunction);

      public:
        static std::vector<std::vector<size_t>> findBorders(const CADacomb::HalfEdgeMesh&, const std::vector<size_t>& trianglesInCluster);
        static std::vector<std::vector<size_t>> splitCluster(const CADacomb::Triangulation&, const std::vector<size_t>& TriangleCluster, const std::vector<size_t>& loopToBreak);
    };
}