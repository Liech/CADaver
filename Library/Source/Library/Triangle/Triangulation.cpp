#include "Triangulation.h"

#include "STLWriter.h"

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

        STLWriter::write(filename, tri);
    }
}