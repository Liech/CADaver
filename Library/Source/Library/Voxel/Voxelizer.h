#pragma once

#include <vector>
#include <memory>
#include <glm/glm.hpp>

//http://research.michael-schwarz.com/publ/files/vox-siga10.pdf (4.1 Direct, triangle-parallel solid voxelization)


namespace Library
{
    class Voxelizer
    {
      public:
        void voxelize(std::vector<bool>& result, const std::vector<glm::dvec3>& vertecies, const std::vector<size_t>& indices, const glm::dvec3& start, const glm::dvec3& end, const glm::u64vec3& resolution);
        void voxelize(std::vector<bool>& result, const std::vector<glm::dvec3>& vertecies, const glm::dvec3& start, const glm::dvec3& end, const glm::u64vec3& resolution);

        void boxelize(const bool* data, const glm::dvec3& start, const glm::dvec3& end, const glm::u64vec3& resolution, std::vector<glm::dvec3>& out_triangles);

      private:
        void spatialHashTriangles(const std::vector<glm::dvec3>& vertecies,
                                  const std::vector<size_t>&     indices,
                                  const glm::dvec3&              start,
                                  const glm::dvec3&              end,
                                  const glm::u64vec3&            resolution,
                                  std::vector<std::vector<size_t>>&);
        void applyTrianglesToVolume(std::vector<bool>& volume,
                                    const std::vector<std::vector<size_t>>&,
                                    const std::vector<glm::dvec3>& vertecies,
                                    const std::vector<size_t>&     indices,
                                    const glm::dvec3&              start,
                                    const glm::dvec3&              end,
                                    const glm::u64vec3&            resolution);

        void box(const glm::dvec3& a, const glm::dvec3& b, const glm::dvec3& c, glm::dvec2& start, glm::dvec2& end);
        bool intersectRayTriangle(const glm::dvec3& rayStart, const glm::dvec3& rayDir, const glm::dvec3& A, const glm::dvec3& B, const glm::dvec3& C, glm::dvec3& out);
    };
}