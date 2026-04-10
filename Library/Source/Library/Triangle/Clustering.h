#pragma once

#include <functional>
#include <vector>

namespace Library
{
    class Triangulation;

    class Clustering
    {
      public:
        // Returns vector of regions consisting of triangle inidices
        static std::vector<std::vector<size_t>> cluster(const Triangulation&, std::function<bool(size_t currentIndex, size_t candidateIndex, const Triangulation&)> growFunction);
        static std::vector<std::vector<size_t>> findHoles(const Triangulation&, const std::vector<size_t>& trianglesInCluster);

    };
}