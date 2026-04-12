#include "BaseShapeGenerator.h"

namespace Library
{
    Triangulation BaseShapeGenerator::cube()
    {
        Triangulation m;
        m.vertices = {
            { 0, 0, 0 },
            { 1, 0, 0 },
            { 1, 1, 0 },
            { 0, 1, 0 },
            { 0, 0, 1 },
            { 1, 0, 1 },
            { 1, 1, 1 },
            { 0, 1, 1 }
        };
        m.indices = { 0, 3, 2, 0, 2, 1, 4, 5, 6, 4, 6, 7, 0, 1, 5, 0, 5, 4, 1, 2, 6, 1, 6, 5, 2, 3, 7, 2, 7, 6, 3, 0, 4, 3, 4, 7 };
        return m;
    }

    Triangulation BaseShapeGenerator::pyramid()
    {
        Triangulation m;
        m.vertices = {
            {   0,   0, 0 },
            {   1,   0, 0 },
            {   1,   1, 0 },
            {   0,   1, 0 }, // Base (0-3)
            { 0.5, 0.5, 1 }  // Apex (4)
        };
        m.indices = {
            0, 2, 1, 0, 3, 2, // Base triangles
            0, 1, 4,          // Side 1
            1, 2, 4,          // Side 2
            2, 3, 4,          // Side 3
            3, 0, 4           // Side 4
        };
        return m;
    }

    Triangulation BaseShapeGenerator::cylinder(size_t segments, float height, float radius)
    {
        Triangulation m;

        // 1. Generate Ring Vertices
        for (int i = 0; i < segments; ++i)
        {
            float theta = 2.0f * 3.1415926f * float(i) / float(segments);
            float x     = radius * cosf(theta);
            float y     = radius * sinf(theta);
            m.vertices.push_back({ x, y, 0 });      // Bottom ring
            m.vertices.push_back({ x, y, height }); // Top ring
        }

        // 2. Add Cap Center Vertices (Hubs)
        size_t bottomCenterIdx = m.vertices.size();
        m.vertices.push_back({ 0, 0, 0 }); // Center of bottom cap
        size_t topCenterIdx = m.vertices.size();
        m.vertices.push_back({ 0, 0, height }); // Center of top cap

        for (int i = 0; i < segments; ++i)
        {
            size_t b0 = i * 2;
            size_t t0 = i * 2 + 1;
            size_t b1 = ((i + 1) % segments) * 2;
            size_t t1 = ((i + 1) % segments) * 2 + 1;

            // Side Faces (The "Sleeve")
            m.indices.insert(m.indices.end(), { b0, b1, t1, b0, t1, t0 });

            // Bottom Cap (Triangle Fan)
            m.indices.insert(m.indices.end(), { bottomCenterIdx, b1, b0 });

            // Top Cap (Triangle Fan)
            m.indices.insert(m.indices.end(), { topCenterIdx, t0, t1 });
        }
        return m;
    }
    Triangulation BaseShapeGenerator::cubeWithHole(size_t segments, float radius)
    {
        Triangulation m;
        if (segments % 4 != 0)
            segments = (segments / 4 + 1) * 4;
        if (radius > 0.45f)
            radius = 0.45f;

        // 1. Outer Vertices
        m.vertices = {
            { 0, 0, 0 },
            { 1, 0, 0 },
            { 1, 1, 0 },
            { 0, 1, 0 }, // 0-3 Bottom (Z=0)
            { 0, 0, 1 },
            { 1, 0, 1 },
            { 1, 1, 1 },
            { 0, 1, 1 }  // 4-7 Top (Z=1)
        };

        // 2. Inner Vertices
        size_t innerStart = m.vertices.size();
        float  startAngle = 3.1415926f * 1.00f; // Aligning with corner 0
        for (size_t i = 0; i < segments; ++i)
        {
            float theta = startAngle + (2.0f * 3.1415926f * (float)i / (float)segments);
            float x     = 0.5f + radius * cosf(theta);
            float y     = 0.5f + radius * sinf(theta);
            m.vertices.push_back({ x, y, 0 }); // Bottom Inner (innerStart + i*2)
            m.vertices.push_back({ x, y, 1 }); // Top Inner (innerStart + i*2 + 1)
        }
        // 3. Outer Walls (Facing OUT)
        m.indices.insert(m.indices.end(),
                         {
                           0, 1, 5, 0, 5, 4, // Front
                           1, 2, 6, 1, 6, 5, // Right
                           2, 3, 7, 2, 7, 6, // Back
                           3, 0, 4, 3, 4, 7  // Left
                         });

        // 4. Inner Bore (Facing INSIDE the hole - outward from the solid)
        for (size_t i = 0; i < segments; ++i)
        {
            uint32_t b0 = (uint32_t)(innerStart + i * 2);
            uint32_t t0 = b0 + 1;
            uint32_t b1 = (uint32_t)(innerStart + ((i + 1) % segments) * 2);
            uint32_t t1 = b1 + 1;

            // To point normals toward the center of the hole:
            // b0 -> b1 -> t1 then b0 -> t1 -> t0
            m.indices.insert(m.indices.end(), { b0, t1,b1 , t1, b0, t0 });
        }

        // 5. Caps (Top and Bottom)
        for (size_t i = 0; i < segments; ++i)
        {
            uint32_t b_in0 = (uint32_t)(innerStart + i * 2);
            uint32_t b_in1 = (uint32_t)(innerStart + ((i + 1) % segments) * 2);
            uint32_t t_in0 = b_in0 + 1;
            uint32_t t_in1 = b_in1 + 1;

            uint32_t corner = (uint32_t)((i * 4) / segments);
            uint32_t b_out  = corner;
            uint32_t t_out  = corner + 4;

            // TOP CAP (Z+) - CCW looking down: Outer -> Inner_Next -> Inner_Curr
            m.indices.insert(m.indices.end(), { t_out, t_in1, t_in0 });

            // BOTTOM CAP (Z-) - CCW looking up: Outer -> Inner_Curr -> Inner_Next
            m.indices.insert(m.indices.end(), { b_out, b_in0, b_in1 });

            // BRIDGE (Filling the square corners)
            if (((i + 1) * 4) / segments > corner)
            {
                uint32_t b_out_next = (uint32_t)((corner + 1) % 4);
                uint32_t t_out_next = b_out_next + 4;

                // TOP BRIDGE: Outer -> OuterNext -> InnerNext
                m.indices.insert(m.indices.end(), { t_out, t_out_next, t_in1 });
                // BOTTOM BRIDGE: Outer -> InnerNext -> OuterNext
                m.indices.insert(m.indices.end(), { b_out, b_in1, b_out_next });
            }
        }

        return m;
    }
    Triangulation BaseShapeGenerator::cone(size_t segments, float radius, float height)
    {
        Triangulation m;
        if (segments < 3)
            segments = 3; // Minimum for a volume

        // 1. Vertices: Bottom Ring
        for (size_t i = 0; i < segments; ++i)
        {
            float theta = 2.0f * 3.1415926f * (float)i / (float)segments;
            m.vertices.push_back({ radius * cosf(theta), radius * sinf(theta), 0 });
        }

        // 2. Apex and Base Center
        size_t apexIdx = m.vertices.size();
        m.vertices.push_back({ 0, 0, height }); // The tip

        size_t centerIdx = m.vertices.size();
        m.vertices.push_back({ 0, 0, 0 }); // Center of the bottom cap

        // 3. Indices
        for (size_t i = 0; i < segments; ++i)
        {
            uint32_t curr = (uint32_t)i;
            uint32_t next = (uint32_t)((i + 1) % segments);

            // Sides: Apex -> Next -> Curr (CCW facing out)
            m.indices.insert(m.indices.end(), { (uint32_t)apexIdx, curr,next });

            // Base Cap: Center -> Curr -> Next (CCW facing down)
            m.indices.insert(m.indices.end(), { (uint32_t)centerIdx, next, curr });
        }

        return m;
    }
}

#ifdef ISTESTPROJECT
#include "Library/catch.hpp"
#include "HalfEdge/HalfEdgeHealth.h"

TEST_CASE("BaseShapeGenerator/unitcube")
{
    auto tri = Library::BaseShapeGenerator::cube();
    REQUIRE(Library::HalfEdgeHealth::isHealthy(tri));
    //tri.saveAsSTL("C:\\Users\\nicol\\Downloads\\cube.stl");
}
TEST_CASE("BaseShapeGenerator/pyramid")
{
    auto tri = Library::BaseShapeGenerator::pyramid();
    REQUIRE(Library::HalfEdgeHealth::isHealthy(tri));
    //tri.saveAsSTL("C:\\Users\\nicol\\Downloads\\pyramid.stl");
}
TEST_CASE("BaseShapeGenerator/cylinder")
{
    auto        tri    = Library::BaseShapeGenerator::cylinder(5, 1, 1);
    std::string report = Library::HalfEdgeHealth::createReport(tri);
    REQUIRE(Library::HalfEdgeHealth::isHealthy(tri));
    //tri.saveAsSTL("C:\\Users\\nicol\\Downloads\\cylinder5.stl");
}
TEST_CASE("BaseShapeGenerator/cubeWithHole")
{
    auto        tri    = Library::BaseShapeGenerator::cubeWithHole(100);
    std::string report = Library::HalfEdgeHealth::createReport(tri);
    REQUIRE(Library::HalfEdgeHealth::isHealthy(tri));
    //tri.saveAsSTL("C:\\Users\\nicol\\Downloads\\cubeWithHole100.stl");
}
TEST_CASE("BaseShapeGenerator/cone")
{
    auto        tri    = Library::BaseShapeGenerator::cone(10,1,1);
    std::string report = Library::HalfEdgeHealth::createReport(tri);
    REQUIRE(Library::HalfEdgeHealth::isHealthy(tri));
    //tri.saveAsSTL("C:\\Users\\nicol\\Downloads\\cone10.stl");
}

#endif