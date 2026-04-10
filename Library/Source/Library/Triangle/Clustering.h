#pragma once

#include <functional>
#include <set>
#include <vector>

namespace Library
{
    class Triangulation;

    class Clustering
    {
      public:
        // Returns vector of regions consisting of triangle inidices
        static std::vector<std::vector<size_t>> cluster(const Triangulation&, std::function<bool(size_t currentIndex, size_t candidateIndex, const Triangulation&)> growFunction);
        //static void                             removeHolesByDivision(const Triangulation&, std::vector<std::vector<size_t>>& clusters);

      private:
        static std::vector<std::vector<size_t>> findLoops(const Triangulation&, const std::vector<size_t>& trianglesInCluster);
        static std::vector<bool>                getLoopDirections(const Triangulation&, const std::vector<std::vector<size_t>>& allLoops, const std::vector<size_t>& trianglesInCluster);
    };
}