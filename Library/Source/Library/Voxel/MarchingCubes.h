#pragma once

#include <vector>
#include <array>
#include <glm/glm.hpp>

//https://paulbourke.net/geometry/polygonise/
class MarchingCubes {
public:
  static std::vector<glm::dvec3> polygonize(const std::vector<bool>&, const glm::dvec3& origin, const glm::dvec3& voxelSize, const glm::u64vec3& resolution);
  static std::vector<glm::dvec3> polygonize(const bool*, const glm::dvec3& origin, const glm::dvec3& voxelSize, const glm::u64vec3& resolution);
  static std::vector<glm::dvec3> polygonize(const std::vector<double>&, const glm::dvec3& origin, const glm::dvec3& voxelSize, const glm::u64vec3& resolution, double isovalue);
  static std::vector<glm::dvec3> polygonize(const std::array<glm::dvec3, 8>& p, const std::array<double, 8>& val, double isolevel);

  //convert binary voxelization to density field for non stair steppy marching cubes
  static std::vector<double> pack(const std::vector<bool>&, const glm::u64vec3& resolution, const glm::u64vec3& oneVoxelSize);
  static std::vector<double> pack(const bool*, const glm::u64vec3& resolution, const glm::u64vec3& oneVoxelSize);
private:
  static glm::dvec3 VertexInterp(double isolevel, const glm::dvec3& p1, const glm::dvec3& p2, double valp1, double valp2);

  static const int edgeTable[256];
  static const int triTable[256][16];
};