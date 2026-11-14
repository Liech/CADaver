#include "Picking/AssetRayPicker/TriangulationRayPicker.h"

#include "CADacomb/Triangle/Triangulation.h"

#include <limits>

TriangulationRayPicker::TriangulationRayPicker() {}

TriangulationRayPicker::~TriangulationRayPicker() {}

CADacomb::AABB3 TriangulationRayPicker::getAABB(const CADacomb::Triangulation& triangulation) const
{
    if (triangulation.vertices.empty())
    {
        return CADacomb::AABB3(glm::dvec3(0.0), glm::dvec3(-1.0));
    }
    auto aabb = triangulation.getAABB();
    return CADacomb::AABB3(aabb.first, aabb.first + aabb.second);
}

PickResult TriangulationRayPicker::pick(const CADacomb::Ray& ray, const CADacomb::Triangulation& triangulation, const CADacomb::GUID& assetGuid, const std::string& assetType) const
{
    PickResult result;
    bool       anyHit          = false;
    double     closestDistance = std::numeric_limits<double>::max();
    size_t     closestTriIndex = 0;

    const size_t indexCount  = triangulation.indices.size();
    const size_t vertexCount = triangulation.vertices.size();

    for (size_t i = 0; i + 2 < indexCount; i += 3)
    {
        size_t i0 = triangulation.indices[i];
        size_t i1 = triangulation.indices[i + 1];
        size_t i2 = triangulation.indices[i + 2];

        if (i0 >= vertexCount || i1 >= vertexCount || i2 >= vertexCount)
        {
            continue;
        }

        double hitDist = 0.0;
        if (intersectTriangle(ray, triangulation.vertices[i0], triangulation.vertices[i1], triangulation.vertices[i2], hitDist))
        {
            if (hitDist < closestDistance)
            {
                closestDistance = hitDist;
                closestTriIndex = i / 3;
                anyHit          = true;
            }
        }
    }

    if (anyHit)
    {
        result.setHit(assetGuid, assetType, closestDistance, ray.getOrigin() + ray.getDirection() * closestDistance);
        result.setTriangleIndex(closestTriIndex);
    }

    return result;
}

bool TriangulationRayPicker::intersectTriangle(const CADacomb::Ray& ray, const glm::dvec3& v0, const glm::dvec3& v1, const glm::dvec3& v2, double& outDistance) const
{
    const double     epsilon = 1e-9;
    const glm::dvec3 edge1   = v1 - v0;
    const glm::dvec3 edge2   = v2 - v0;
    const glm::dvec3 pVec    = glm::cross(ray.getDirection(), edge2);
    const double     det     = glm::dot(edge1, pVec);

    if (glm::abs(det) < epsilon)
    {
        return false;
    }

    const double     invDet = 1.0 / det;
    const glm::dvec3 tVec   = ray.getOrigin() - v0;
    const double     u      = glm::dot(tVec, pVec) * invDet;
    if (u < 0.0 || u > 1.0)
    {
        return false;
    }

    const glm::dvec3 qVec = glm::cross(tVec, edge1);
    const double     v    = glm::dot(ray.getDirection(), qVec) * invDet;
    if (v < 0.0 || u + v > 1.0)
    {
        return false;
    }

    const double t = glm::dot(edge2, qVec) * invDet;
    if (t <= epsilon)
    {
        return false;
    }

    outDistance = t;
    return true;
}
