#pragma once

#include <memory>
#include <vector>
#include <map>
#include <glm/glm.hpp>

namespace Library
{
    class Triangulation;
    class BinaryVolume;

    struct IVec3Compare
    {
        bool operator()(const glm::ivec3& a, const glm::ivec3& b) const
        {
            if (a.x != b.x)
            {
                return a.x < b.x;
            }
            if (a.y != b.y)
            {
                return a.y < b.y;
            }
            return a.z < b.z;
        }
    };

    class BlockyVoxelTriangulation
    {
      public:
        BlockyVoxelTriangulation(const BinaryVolume&);

        std::unique_ptr<Triangulation> triangulate();

      private:
        void scan(size_t scanDimension, bool reverse);

        const BinaryVolume& input;

        std::map<glm::ivec3, size_t, IVec3Compare> indexMap;
        size_t                       nextIndex = 0;
        std::vector<size_t>          indexBuffer;
    };
}