#include "Triangulation.h"

#include "STLWriter.h"
#include "Util/stl_reader.h"

namespace Library
{
    Triangulation::Triangulation() {}
    Triangulation::~Triangulation() {}

    void Triangulation::saveAsSTL(const std::string& filename) const
    {
        std::vector<glm::dvec3> tri;
        tri.resize(indices.size());
        for (size_t i = 0; i < indices.size(); i += 3)
        {
            tri[i]     = vertices[indices[i]];
            tri[i + 1] = vertices[indices[i + 2]];
            tri[i + 2] = vertices[indices[i + 1]];
        }
        std::reverse(tri.begin(), tri.end());
        STLWriter::write(filename, tri);
    }

    std::unique_ptr<Triangulation> Triangulation::fromSTLFile(const std::string& filename)
    {
        std::unique_ptr<Triangulation> result = std::make_unique<Triangulation>();

        std::vector<float>        coords;
        std::vector<float>        normals;
        std::vector<unsigned int> tris;
        std::vector<unsigned int> solidRanges;

        try
        {
            stl_reader::ReadStlFile(filename.c_str(),
                                    coords,
                                    normals, // Pass normals container
                                    tris,
                                    solidRanges // Pass solidRanges container
            );
        }
        catch (const std::exception& e)
        {
            // load error
            return nullptr;
        }

        if (coords.empty() || tris.empty())
        {
            // no content
            return nullptr;
        }

        result->vertices.reserve(coords.size() / 3);
        for (size_t i = 0; i < coords.size(); i += 3)
        {
            result->vertices.emplace_back(coords[i + 0], coords[i + 1], coords[i + 2]);
        }

        result->indices.reserve(tris.size());
        for (size_t i = 0; i < tris.size(); i += 3)
        {
            result->indices.push_back(static_cast<int>(tris[i + 0]));
            result->indices.push_back(static_cast<int>(tris[i + 2]));
            result->indices.push_back(static_cast<int>(tris[i + 1]));
        }

        return std::move(result);
    }

    std::pair<glm::dvec3, glm::dvec3> Triangulation::getAABB() const
    {
        auto       inf = std::numeric_limits<double>::infinity();
        glm::dvec3 min = glm::dvec3(inf, inf, inf);
        glm::dvec3 max = glm::dvec3(-inf, -inf, -inf);

        for (const auto& x : vertices)
        {
            min.x = std::min(x.x, min.x);
            min.y = std::min(x.y, min.y);
            min.z = std::min(x.z, min.z);
            max.x = std::max(x.x, max.x);
            max.y = std::max(x.y, max.y);
            max.z = std::max(x.z, max.z);
        }
        glm::dvec3 position = glm::dvec3(min.x, min.y, min.z);
        glm::dvec3 size     = glm::dvec3(max.x - min.x, max.y - min.y, max.z - min.z);
        return std::make_pair(position, size);
    }

    std::map<std::pair<size_t, size_t>, std::vector<size_t>> Triangulation::getAdjacency() const
    {
        std::map<std::pair<size_t, size_t>, std::vector<size_t>> result;
        for (size_t i = 0; i < indices.size(); i += 3)
        {
            size_t t       = i / 3;
            auto   addEdge = [&](size_t a, size_t b)
            {
                if (a > b)
                    std::swap(a, b);
                result[{ a, b }].push_back(t);
            };
            addEdge(indices[i], indices[i + 1]);
            addEdge(indices[i + 1], indices[i + 2]);
            addEdge(indices[i + 2], indices[i]);
        }
        return result;
    }

    glm::dvec3 Triangulation::getFaceNormal(size_t faceIndex) const
    {
        // 1. Get the indices of the three vertices for this face
        // Each face has 3 indices, so face 0 uses 0,1,2; face 1 uses 3,4,5...
        size_t idx0 = indices[faceIndex * 3 + 0];
        size_t idx1 = indices[faceIndex * 3 + 1];
        size_t idx2 = indices[faceIndex * 3 + 2];

        // 2. Retrieve the actual vertex positions
        const glm::dvec3& p0 = vertices[idx0];
        const glm::dvec3& p1 = vertices[idx1];
        const glm::dvec3& p2 = vertices[idx2];

        // 3. Calculate two edge vectors originating from p0
        glm::dvec3 edge1 = p1 - p0;
        glm::dvec3 edge2 = p2 - p0;

        // 4. Compute the cross product
        // The order (edge1 x edge2) follows the Right-Hand Rule
        // based on the winding order of your indices.
        glm::dvec3 normal = glm::cross(edge1, edge2);

        // 5. Normalize the vector
        // We check the length to avoid division by zero on degenerate (zero-area) triangles.
        double length = glm::length(normal);
        if (length > 1e-12)
        {
            return normal / length;
        }

        // Return a zero vector if the triangle is degenerate
        return glm::dvec3(0.0, 0.0, 0.0);
    }
}