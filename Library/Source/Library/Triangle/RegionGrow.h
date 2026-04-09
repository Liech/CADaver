#pragma once

#include <vector>
#include <functional>

namespace Library
{
    class Triangulation;

  class RegionGrow
  {
    public:
      // Returns vector of regions consisting of triangle inidices
      static std::vector<std::vector<size_t>> grow(const Triangulation&, std::function<bool(size_t currentIndex, size_t candidateIndex, const Triangulation&)> growFunction);
  };
}