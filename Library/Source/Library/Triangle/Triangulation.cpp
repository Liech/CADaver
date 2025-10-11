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
        for (unsigned int index : tris)
        {
            result->indices.push_back(static_cast<int>(index));
        }

        return std::move(result);
    }
}