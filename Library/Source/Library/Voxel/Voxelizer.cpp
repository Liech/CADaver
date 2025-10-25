#include "Voxelizer.h"

#include <algorithm>

namespace Library
{
    struct EpsilonEqualityPredicate
    {
        double epsilon;

        EpsilonEqualityPredicate(double epsilon)
          : epsilon(epsilon)
        {
        }

        bool operator()(double a, double b) const
        {
            return std::abs(a - b) < epsilon;
        }
    };

    void Voxelizer::voxelize(std::vector<bool>&             result,
                             const std::vector<glm::dvec3>& vertecies,
                             const std::vector<size_t>&     indices,
                             const glm::dvec3&              start,
                             const glm::dvec3&              end,
                             const glm::u64vec3&            resolution)
    {
        assert(resolution.z % 8 == 0);
        size_t     memorySize = (size_t)resolution.x * (size_t)resolution.y * (size_t)resolution.z;
        glm::dvec3 span       = end - start;

        result.resize(memorySize);
        for (size_t i = 0; i < memorySize; i++)
            result[i] = false;

        std::vector<std::vector<size_t>> triangleStack;
        triangleStack.resize(resolution.x * resolution.y);

        spatialHashTriangles(vertecies, indices, start, end, resolution, triangleStack);
        applyTrianglesToVolume(result, triangleStack, vertecies, indices, start, end, resolution);
    }

    void Voxelizer::voxelize(std::vector<bool>& result, const std::vector<glm::dvec3>& vertecies, const glm::dvec3& start, const glm::dvec3& end, const glm::u64vec3& resolution)
    {
        assert(resolution.z % 8 == 0);
        size_t     memorySize = (size_t)resolution.x * (size_t)resolution.y * (size_t)resolution.z;
        glm::dvec3 span       = end - start;

        result.resize(memorySize);
        for (size_t i = 0; i < memorySize; i++)
            result[i] = false;

        std::vector<std::vector<size_t>> triangleStack;
        triangleStack.resize(resolution.x * resolution.y);
        std::vector<size_t> indices;
        indices.resize(vertecies.size());
        for (size_t i = 0; i < indices.size(); i++)
            indices[i] = i;

        spatialHashTriangles(vertecies, indices, start, end, resolution, triangleStack);
        applyTrianglesToVolume(result, triangleStack, vertecies, indices, start, end, resolution);
    }

    void Voxelizer::applyTrianglesToVolume(std::vector<bool>&                      data,
                                           const std::vector<std::vector<size_t>>& triangleStack,
                                           const std::vector<glm::dvec3>&          vertecies,
                                           const std::vector<size_t>&              indices,
                                           const glm::dvec3&                       start,
                                           const glm::dvec3&                       end,
                                           const glm::u64vec3&                     resolution)
    {
        glm::dvec3               span         = end - start;
        glm::dvec3               rayDirection = glm::dvec3(0, 0, 1);
        EpsilonEqualityPredicate predicate(1e-6);

#pragma omp parallel for
        for (long long x = 0; x < resolution.x; x++)
        {
            for (size_t y = 0; y < resolution.y; y++)
            {
                size_t stackAddress = x + resolution.x * y;
                size_t resultOffset = resolution.z * resolution.y * x + resolution.z * y;
                auto&  stack        = triangleStack[stackAddress];

                double zMin = std::numeric_limits<double>::max();
                for (const auto& x : stack)
                {
                    zMin = std::min(zMin, vertecies[indices[x + 2]].z);
                }

                glm::dvec3 rayStart = start + glm::dvec3(span.x * ((double)x / (double)resolution.x), span.y * ((double)y / (double)resolution.y), 0) - rayDirection * 10.0;
                rayStart.z          = zMin - rayDirection.z;
                std::vector<double> intersections;
                for (auto& tri : stack)
                {
                    const glm::dvec3& a = vertecies[indices[tri]];
                    const glm::dvec3& b = vertecies[indices[tri + 1]];
                    const glm::dvec3& c = vertecies[indices[tri + 2]];
                    glm::dvec3        intersectionLocation;
                    bool              doesIntersect = intersectRayTriangle(rayStart, rayDirection, a, b, c, intersectionLocation);
                    if (doesIntersect)
                    {
                        intersections.push_back(intersectionLocation.z);
                    }
                }
                intersections.erase(std::unique(intersections.begin(), intersections.end(), predicate), intersections.end());

                for (auto& zIntersection : intersections)
                {
                    long long zPos = std::floor(((zIntersection - start.z) / span.z) * (long long)resolution.z);
                    zPos           = std::max((long long)0, zPos);

                    size_t memoryAddress = zPos + resultOffset;
                    size_t max           = resultOffset + resolution.z;
                    for (size_t i = memoryAddress; i < max; i++)
                        data[i] = data[i] ^ true;
                }
            }
        }
    }

    void Voxelizer::spatialHashTriangles(const std::vector<glm::dvec3>&    vertecies,
                                         const std::vector<size_t>&        indices,
                                         const glm::dvec3&                 start,
                                         const glm::dvec3&                 end,
                                         const glm::u64vec3&               resolution,
                                         std::vector<std::vector<size_t>>& triangleStack)
    {
        size_t     amountTriangles = indices.size() / 3;
        glm::dvec3 span            = end - start;

        for (size_t i = 0; i < amountTriangles; i++)
        {
            size_t            address   = i * 3;
            const glm::dvec3& a         = vertecies[indices[address]];
            const glm::dvec3& b         = vertecies[indices[address + 1]];
            const glm::dvec3& c         = vertecies[indices[address + 2]];
            double            planarity = glm::cross(glm::normalize(a - b), glm::normalize(c - b)).z;
            if (std::abs(planarity) < 1e-6)
                continue;
            glm::dvec2 out_start;
            glm::dvec2 out_end;
            box(vertecies[indices[address + 0]], vertecies[indices[address + 1]], vertecies[indices[address + 2]], out_start, out_end);
            size_t xStart = std::floor(((out_start.x - start.x) / span.x) * resolution.x);
            size_t yStart = std::ceil(((out_start.y - start.y) / span.y) * resolution.y);
            size_t xEnd   = std::floor(((out_end.x - start.x) / span.x) * resolution.x);
            size_t yEnd   = std::ceil(((out_end.y - start.y) / span.y) * resolution.y);
            if (xEnd >= resolution.x)
                xEnd = resolution.x - 1;
            if (yEnd >= resolution.y)
                yEnd = resolution.y - 1;

            for (size_t x = xStart; x <= xEnd; x++)
                for (size_t y = yStart; y <= yEnd; y++)
                {
                    triangleStack[y * resolution.x + x].push_back(address);
                }
        }
    }

    void Voxelizer::box(const glm::dvec3& a, const glm::dvec3& b, const glm::dvec3& c, glm::dvec2& start, glm::dvec2& end)
    {
        double minX = (a.x < b.x) ? ((a.x < c.x) ? a.x : c.x) : ((b.x < c.x) ? b.x : c.x);
        double minY = (a.y < b.y) ? ((a.y < c.y) ? a.y : c.y) : ((b.y < c.y) ? b.y : c.y);
        double maxX = (a.x > b.x) ? ((a.x > c.x) ? a.x : c.x) : ((b.x > c.x) ? b.x : c.x);
        double maxY = (a.y > b.y) ? ((a.y > c.y) ? a.y : c.y) : ((b.y > c.y) ? b.y : c.y);
        start       = glm::dvec2(minX, minY);
        end         = glm::dvec2(maxX, maxY);
    }

    bool Voxelizer::intersectRayTriangle(const glm::dvec3& rayOrigin, const glm::dvec3& rayVector, const glm::dvec3& vertex0, const glm::dvec3& vertex1, const glm::dvec3& vertex2, glm::dvec3& out)
    {
        // https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
        const double EPSILON = 1e-16;
        glm::dvec3   edge1, edge2, h, s, q;
        double       a, f, u, v;
        edge1 = vertex1 - vertex0;
        edge2 = vertex2 - vertex0;
        h     = glm::cross(rayVector, edge2);
        a     = glm::dot(edge1, h);

        if (a > -EPSILON && a < EPSILON)
            return false; // This ray is parallel to this triangle.

        f = 1.0 / a;
        s = rayOrigin - vertex0;
        u = f * glm::dot(s, h);

        if (u < -EPSILON || u > 1.0 + EPSILON)
            return false;

        q = glm::cross(s, edge1);
        v = f * glm::dot(rayVector, q);

        if (v < -EPSILON || u + v > 1.0 + EPSILON)
            return false;

        // At this stage we can compute t to find out where the intersection point is on the line.
        double t = f * glm::dot(edge2, q);

        if (t > EPSILON) // ray intersection
        {
            out = rayOrigin + rayVector * t;
            return true;
        }
        else // This means that there is a line intersection but not a ray intersection.
            return false;
    }

    void Voxelizer::boxelize(const bool* data, const glm::dvec3& start, const glm::dvec3& end, const glm::u64vec3& resolution, std::vector<glm::dvec3>& out_triangles)
    {
        glm::dvec3 span      = end - start;
        glm::dvec3 voxelSize = glm::dvec3(span.x / (double)resolution.x, span.y / (double)resolution.y, span.z / (double)resolution.z);
        size_t     dataSize  = (size_t)resolution.x * (size_t)resolution.y * (size_t)resolution.z;
        for (size_t i = 0; i < dataSize; i++)
        {
            if (!data[i])
                continue;
            size_t     x     = i / (resolution.y * resolution.z);
            size_t     y     = (i % (resolution.y * resolution.z)) / resolution.z;
            size_t     z     = i % resolution.z;
            glm::dvec3 point = start + glm::dvec3(x * voxelSize.x, y * voxelSize.y, z * voxelSize.z) - glm::dvec3(0.5, 0.5, -0.5);

            // bard
            glm::dvec3 p0 = point + glm::dvec3(0, 0, 0);
            glm::dvec3 p1 = point + glm::dvec3(voxelSize.x, 0, 0);
            glm::dvec3 p2 = point + glm::dvec3(voxelSize.x, voxelSize.y, 0);
            glm::dvec3 p3 = point + glm::dvec3(0, voxelSize.y, 0);
            glm::dvec3 p4 = point + glm::dvec3(0, 0, voxelSize.z);
            glm::dvec3 p5 = point + glm::dvec3(voxelSize.x, 0, voxelSize.z);
            glm::dvec3 p6 = point + glm::dvec3(voxelSize.x, voxelSize.y, voxelSize.z);
            glm::dvec3 p7 = point + glm::dvec3(0, voxelSize.y, voxelSize.z);

            std::vector<glm::dvec3> box = { p1, p0, p2, p3, p2, p0, p4, p5, p6, p6, p7, p4, p4, p0, p5, p1, p5, p0, p6, p2, p7, p3, p7, p2, p3, p0, p7, p4, p7, p0, p5, p1, p6, p2, p6, p1 };

            out_triangles.insert(out_triangles.begin(), box.begin(), box.end());
        }
    }
}