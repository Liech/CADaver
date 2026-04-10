#pragma once

#include <functional>
#include <set>
#include <vector>

namespace Library
{
    class Triangulation;
    struct HalfEdgeMesh;

    class Clustering
    {
      public:
        // Returns vector of regions consisting of triangle inidices
        static std::vector<std::vector<size_t>> cluster(const Triangulation&, std::function<bool(size_t currentIndex, size_t candidateIndex, const Triangulation&)> growFunction);
        static std::vector<std::vector<size_t>> findBorders(const HalfEdgeMesh&, const std::vector<size_t>& trianglesInCluster);
    };
}