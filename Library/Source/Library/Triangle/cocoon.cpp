#include "cocoon.h"

#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepLib.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <BRep_Tool.hxx>
#include <GeomAPI_PointsToBSplineSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Surface.hxx>
#include <ShapeFix_Face.hxx>
#include <Standard_Handle.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <gp_Pnt.hxx>

namespace Library
{
    TopoDS_Face cocoon::convert(const TopoDS_Wire& wire)
    {
        std::vector<glm::dvec3> allVertices;

        // 1. Extract vertices in order
        // WireExplorer ensures we follow the path of the wire, not just the internal heap order
        for (BRepTools_WireExplorer exp(wire); exp.More(); exp.Next())
        {
            TopoDS_Vertex v = exp.CurrentVertex();
            gp_Pnt        p = BRep_Tool::Pnt(v);
            allVertices.push_back({ (double)p.X(), (double)p.Y(), (double)p.Z() });
        }
        return convert(allVertices);
    }

    TopoDS_Face cocoon::convert(const std::vector<glm::dvec3>& wire)
    {
        std::string dbg;
        dbg += "cocoon::convert Input:\n";
        for (const auto& x : wire)
            dbg += std::to_string(x.x) + " ; " + std::to_string(x.y) + " ; " + std::to_string(x.z) + "\n";

        auto sections = cutWire(wire);
        if (sections.size() != 4)
            return fallback(wire);
        assert(sections.size() == 4);
        auto gridSize     = getGridSize(sections);
        auto gridSections = fixSections(sections, gridSize);

        for (const auto& sec : gridSections)
        {
            dbg += "SECTION:\n";
            for (const auto& x : sec)
                dbg += std::to_string(x.x) + " ; " + std::to_string(x.y) + " ; " + std::to_string(x.z) + "\n";
        }

        auto coonGrid = calculateCoon(gridSections, gridSize);
        auto laplaceGrid = calculateLaplace(wire);
        return toFace(laplaceGrid);
    }

    TopoDS_Face cocoon::fallback(const std::vector<glm::dvec3>& points)
    {
        // 1. Safety check: Need at least 3 points to define an area
        if (points.size() < 3)
        {
            return TopoDS_Face();
        }

        // 2. Build the wire from the points
        BRepBuilderAPI_MakePolygon polygonMaker;
        for (const auto& p : points)
        {
            polygonMaker.Add(gp_Pnt(p.x, p.y, p.z));
        }

        // Close the loop to ensure it forms a valid boundary
        polygonMaker.Close();

        if (!polygonMaker.IsDone())
        {
            return TopoDS_Face();
        }

        TopoDS_Wire wire = polygonMaker.Wire();

        // 3. Create a planar face bounded by this wire
        // Standard_True forces the builder to find the best-fit plane
        BRepBuilderAPI_MakeFace faceMaker(wire, Standard_True);

        if (!faceMaker.IsDone())
        {
            return TopoDS_Face();
        }

        TopoDS_Face face = faceMaker.Face();

        // 4. Fix orientation
        // For flat strips, the normal might be flipped relative to your expectations.
        // ShapeFix_Face ensures the wire orientation matches the surface normal.
        Handle(ShapeFix_Face) sff = new ShapeFix_Face(face);
        sff->Perform();

        return sff->Face();
    }

    TopoDS_Face cocoon::toFace(std::vector<std::vector<glm::dvec3>> grid)
    {
        std::string dbg = grid2string(grid);
        // 1. Dimensionen des Grids bestimmen
        int         numU = static_cast<int>(grid.size());
        if (numU == 0)
            return TopoDS_Face();
        int numV = static_cast<int>(grid[0].size());

        // 2. Das OCCT-spezifische 2D-Array erstellen
        // Hinweis: OCCT Arrays sind 1-basiert (Index 1 bis n)
        TColgp_Array2OfPnt pointArray(1, numU, 1, numV);

        for (int i = 0; i < numU; ++i)
        {
            for (int j = 0; j < numV; ++j)
            {
                const glm::dvec3& p = grid[i][j];
                pointArray.SetValue(i + 1, j + 1, gp_Pnt(p.x, p.y, p.z));
            }
        }

        // 3. Eine BSpline-Fläche durch die Punkte interpolieren
        // Wir nutzen GeomAPI_PointsToBSplineSurface für eine glatte Approximation
        // Parameter: (Punkte, MinDegree, MaxDegree, Continuity, Tolerance)
        GeomAPI_PointsToBSplineSurface interpolator(pointArray, 3, 3, GeomAbs_C1, 1.0e-3);

        if (!interpolator.IsDone())
        {
            // Fallback, falls die Interpolation fehlschlägt
            return TopoDS_Face();
        }
        Handle(Geom_BSplineSurface) surface = interpolator.Surface();

        // Get the parametric bounds of the surface
        Standard_Real uMin, uMax, vMin, vMax;
        surface->Bounds(uMin, uMax, vMin, vMax);

        // Create a face using the surface's own natural boundaries
        // This is much more stable than re-using an external wire.
        BRepBuilderAPI_MakeFace faceMaker(surface, uMin, uMax, vMin, vMax, 1e-6);

        if (!faceMaker.IsDone())
        {
            return TopoDS_Face();
        }

        TopoDS_Face finalFace = faceMaker.Face();

        // Ensure the orientation is correct for area calculations
        ShapeFix_Face fixer(finalFace);
        fixer.Perform();
        return fixer.Face();
    }

    std::vector<std::vector<glm::dvec3>> cocoon::calculateLaplace(std::vector<glm::dvec3> loop)
    {
        // 1. Divide the loop into 4 logical sides (using your existing cutWire logic)
        auto sections = cutWire(loop);
        if (sections.size() != 4)
            return {};

        auto gridSize = getGridSize(sections);
        // Ensure sides match the expected grid dimensions (using your fixSections)
        auto gridSections = fixSections(sections, gridSize);

        int numU = gridSize.x;
        int numV = gridSize.y;

        // 2. Initialize the grid and fill the boundaries
        std::vector<std::vector<glm::dvec3>> grid(numU, std::vector<glm::dvec3>(numV));

        // Map boundaries to the grid edges
        for (int i = 0; i < numU; ++i)
        {
            grid[i][0]        = gridSections[0][i]; // Bottom
            grid[i][numV - 1] = gridSections[2][i]; // Top
        }
        for (int j = 0; j < numV; ++j)
        {
            grid[numU - 1][j] = gridSections[1][j]; // Right
            grid[0][j]        = gridSections[3][j]; // Left
        }

        // 3. Initial Guess for Interior (Bilinear Average)
        // We need a non-zero starting point so the mesh isn't collapsed
        for (int i = 1; i < numU - 1; ++i)
        {
            double u = (double)i / (numU - 1);
            for (int j = 1; j < numV - 1; ++j)
            {
                double v = (double)j / (numV - 1);

                glm::dvec3 horizontal = glm::mix(grid[0][j], grid[numU - 1][j], u);
                glm::dvec3 vertical   = glm::mix(grid[i][0], grid[i][numV - 1], v);
                grid[i][j]            = (horizontal + vertical) * 0.5;
            }
        }

        // 4. Laplacian Smoothing Iterations
        // This "untwists" the grid regardless of the initial guess quality.
        // 50-100 iterations is usually enough for small CAD patches.
        const int iterations = 100;
        for (int iter = 0; iter < iterations; ++iter)
        {
            // We only move the interior points (1 to n-1)
            for (int i = 1; i < numU - 1; ++i)
            {
                for (int j = 1; j < numV - 1; ++j)
                {
                    // Standard 4-point Laplacian operator
                    grid[i][j] = (grid[i - 1][j] + grid[i + 1][j] + grid[i][j - 1] + grid[i][j + 1]) * 0.25;
                }
            }
        }

        return grid;
    }

    std::vector<std::vector<glm::dvec3>> cocoon::calculateCoon(std::vector<std::vector<glm::dvec3>> sections, glm::ivec2 resolution)
    {
        assert(sections.size() == 4);
        std::reverse(sections[2].begin(), sections[2].end());
        std::reverse(sections[3].begin(), sections[3].end());

        std::string dbg = "Sides:";

        for (const auto& sec : sections)
        {
            dbg += "  SIDE:\n";
            for (const auto& x : sec)
                dbg += "    " + std::to_string(x.x) + " ; " + std::to_string(x.y) + " ; " + std::to_string(x.z) + "\n";
        }

        // Initialisiere das Grid: [U][V]
        std::vector<std::vector<glm::dvec3>> grid(resolution.x, std::vector<glm::dvec3>(resolution.y));

        // Wir definieren die 4 Grenzkurven zur besseren Lesbarkeit:
        // C0(u) -> unten (V=0)
        // C1(v) -> rechts (U=1)
        // C2(u) -> oben   (V=1)
        // C3(v) -> links  (U=0)
        const auto& C0 = sections[0];
        const auto& C1 = sections[1];
        const auto& C2 = sections[2];
        const auto& C3 = sections[3];

        // Die 4 Eckpunkte für das Tensor-Produkt
        glm::dvec3 P00 = C0.front(); // Unten-Links
        glm::dvec3 P10 = C0.back();  // Unten-Rechts
        glm::dvec3 P11 = C2.back();  // Oben-Rechts (Annahme: C2 läuft parallel zu C0)
        glm::dvec3 P01 = C2.front(); // Oben-Links

        for (int i = 0; i < resolution.x; ++i)
        {
            double u = static_cast<double>(i) / (resolution.x - 1);

            for (int j = 0; j < resolution.y; ++j)
            {
                double v = static_cast<double>(j) / (resolution.y - 1);

                // 1. Interpolation zwischen den U-Kurven (C0 und C2)
                // Wir nutzen direkt die Punkte aus dem fixSections-Ergebnis
                glm::dvec3 L_u = (1.0f - v) * C0[i] + v * C2[i];

                // 2. Interpolation zwischen den V-Kurven (C3 und C1)
                glm::dvec3 L_v = (1.0f - u) * C3[j] + u * C1[j];

                // 3. Bilineare Interpolation der 4 Eckpunkte (Korrekturterm)
                glm::dvec3 B_uv = (1.0f - u) * (1.0f - v) * P00 + u * (1.0f - v) * P10 + (1.0f - u) * v * P01 + u * v * P11;

                // Coons-Fläche: Summe der Regelflächen minus Korrektur
                grid[i][j] = L_u + L_v - B_uv;
            }
        }
        // Post-processing Smoothing
        for (int iter = 0; iter < 15; ++iter)
        {
            for (int i = 1; i < resolution.x - 1; ++i)
            {
                for (int j = 1; j < resolution.y - 1; ++j)
                {
                    grid[i][j] = (grid[i - 1][j] + grid[i + 1][j] + grid[i][j - 1] + grid[i][j + 1]) * 0.25;
                }
            }
        }
        return grid;
    }

    std::vector<std::vector<glm::dvec3>> cocoon::fixSections(std::vector<std::vector<glm::dvec3>> sections, glm::ivec2 resolution)
    {
        assert(sections.size() == 4);

        for (int i = 0; i < 4; ++i)
        {
            // Bestimme das Ziel für diesen Abschnitt:
            // Index 0 & 2 (U-Richtung) -> resolution.x
            // Index 1 & 3 (V-Richtung) -> resolution.y
            int targetCount = (i % 2 == 0) ? resolution.x : resolution.y;

            auto& currentSide = sections[i];

            // Falls Punkte fehlen, "erfinden" wir sie auf der letzten Linie
            if (currentSide.size() < static_cast<size_t>(targetCount))
            {
                // Wir nehmen die letzten beiden Punkte (die letzte Linie)
                glm::dvec3 pLast        = currentSide.back();
                glm::dvec3 pPenultimate = currentSide[currentSide.size() - 2];

                // Wir berechnen die Mitte dieser Linie
                glm::dvec3 midPoint = (pLast + pPenultimate) * 0.5;

                // Wir fügen den Punkt VOR dem letzten Punkt ein,
                // um die Kurve "feiner" zu unterteilen, ohne das Ende zu verschieben
                currentSide.insert(currentSide.end() - 1, midPoint);
            }
        }

        return sections;
    }

    glm::ivec2 cocoon::getGridSize(const std::vector<std::vector<glm::dvec3>>& quarters)
    {
        assert(quarters.size() == 4);

        // Die gegenüberliegenden Seiten im Coons-Patch sind:
        // Seite 0 (unten) vs. Seite 2 (oben) -> bestimmt die U-Auflösung
        // Seite 1 (rechts) vs. Seite 3 (links) -> bestimmt die V-Auflösung
        // (Reihenfolge hängt von deiner Wire-Zerlegung ab, üblich ist CCW)

        size_t countS0 = quarters[0].size();
        size_t countS1 = quarters[1].size();
        size_t countS2 = quarters[2].size();
        size_t countS3 = quarters[3].size();

        // Wir nehmen das Maximum der gegenüberliegenden Kurven,
        // damit kein Detail verloren geht.
        int gridU = static_cast<int>(std::max(countS0, countS2));
        int gridV = static_cast<int>(std::max(countS1, countS3));

        return glm::ivec2(gridU, gridV);
    }

    std::vector<std::vector<glm::dvec3>> cocoon::cutWire(const std::vector<glm::dvec3>& allVertices)
    {
        size_t                               n = allVertices.size();
        std::vector<std::vector<glm::dvec3>> sections;

        if (n < 4)
        {
            return { allVertices };
        }

        // 2. Divide into 4 sections based on vertex count
        // We calculate the indices where each new section should start
        for (int i = 0; i < 4; ++i)
        {
            size_t start = (i * n) / 4;
            size_t end   = ((i + 1) * n) / 4;

            std::vector<glm::dvec3> section;
            for (size_t j = start; j <= end && j < n; ++j)
            {
                section.push_back(allVertices[j]);
            }

            // To make the sections "continuous", the last point of section i
            // is often the first point of section i+1.
            // If the wire is closed, the very last section should ideally
            // include the first vertex to "close" the loop.
            if (i == 3)
            {
                section.push_back(allVertices[0]);
            }

            sections.push_back(section);
        }

        return sections;
    }

    std::string cocoon::grid2string(const std::vector<std::vector<glm::dvec3>>& grid)
    {
        std::stringstream ss;

        int numU = static_cast<int>(grid.size());
        if (numU == 0)
            return "Empty Grid";
        int numV = static_cast<int>(grid[0].size());

        ss << "--- Grid Dimensions: [" << numU << " x " << numV << "] ---\n";

        for (int i = 0; i < numU; ++i)
        {
            ss << "Row " << std::setw(2) << i << ": ";
            for (int j = 0; j < numV; ++j)
            {
                const auto& p = grid[i][j];
                // Format: (x, y, z)
                ss << "(" << std::fixed << std::setprecision(3) << p.x << ", " << p.y << ", " << p.z << ") ";

                if (j < numV - 1)
                    ss << "| ";
            }
            ss << "\n";
        }
        ss << "--------------------------------------";

        return ss.str();
    }
}

#ifdef ISTESTPROJECT
#include "Library/catch.hpp"
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>
#include <TopoDS.hxx>
using namespace Library;

TEST_CASE("cocoon::convert - Unit Square Conversion", "[cocoon][opencascade]")
{
    // 1. Setup: Create a unit square wire (0,0) to (1,1) in the XY plane
    std::vector<glm::dvec3> wire;
    wire.push_back(glm::dvec3(0, 0, 0));
    wire.push_back(glm::dvec3(1, 0, 0));
    wire.push_back(glm::dvec3(1, 1, 0));
    wire.push_back(glm::dvec3(0, 1, 0));

    SECTION("Conversion produces a valid non-null face")
    {
        TopoDS_Face result = cocoon::convert(wire);

        REQUIRE(!result.IsNull());
        CHECK(result.ShapeType() == TopAbs_FACE);
    }

    SECTION("Converted face has the correct area")
    {
        TopoDS_Face result = cocoon::convert(wire);

        // Calculate mass properties to verify area
        GProp_GProps properties;
        BRepGProp::LinearProperties(result, properties); // For wire length (optional)
        BRepGProp::SurfaceProperties(result, properties);

        double area = properties.Mass();

        // Check if area is 1.0 within a small epsilon
        REQUIRE(area == Approx(1.0).margin(1e-6));
    }
}

TEST_CASE("cocoon::convert - Public Halfpipe Test", "[cocoon][opencascade]")
{
    // --- 1. Setup: Generate a circular loop for a halfpipe ---
    // A halfpipe consists of two semi-circles connected by two straight lines.
    // Radius = 1.0, Length = 1.0
    const int               arcResolution = 5;
    const double            radius        = 1.0;
    const double            length        = 1.0;
    std::vector<glm::dvec3> boundaryLoop;

    // --- SIDE A: Semi-circle at Y = 0 (Lower Arch) ---
    // We go from 0 to PI.
    // This includes the start point (1, 0, 0) and the end point (-1, 0, 0)
    for (int i = 0; i < arcResolution; ++i)
    {
        double t     = (double)i / (arcResolution - 1);
        double angle = t * M_PI;
        boundaryLoop.push_back(glm::dvec3(radius * std::cos(angle), 0.0, radius * std::sin(angle)));
    }

    // --- SIDE B: Straight line at X = -radius ---
    // The point (-radius, 0, 0) is already added by the loop above.
    // We only add the intermediate points and the final corner.
    // If your resolution is low, just adding the next corner is enough.
    boundaryLoop.push_back(glm::dvec3(-radius, length, 0.0));

    // --- SIDE C: Semi-circle at Y = length (Upper Arch) ---
    // We are currently at (-radius, length, 0).
    // The next arc needs to start here and move toward (+radius, length, 0).
    // That means moving from angle PI back to 0.
    // We skip i=0 because (-radius, length, 0) was just added.
    for (int i = 1; i < arcResolution; ++i)
    {
        double t     = (double)i / (arcResolution - 1);
        double angle = M_PI - (t * M_PI); // Starts at PI, ends at 0
        boundaryLoop.push_back(glm::dvec3(radius * std::cos(angle), length, radius * std::sin(angle)));
    }

    // --- SIDE D: Straight line at X = +radius ---
    // We are currently at (radius, length, 0).
    // The loop needs to close back at (radius, 0, 0).
    // (radius, 0, 0) is already the VERY FIRST point in the vector,
    // so we don't add it again. We just let the loop "end" here.
    // If you want a point in the middle of this straight edge:
    // boundaryLoop.push_back(glm::dvec3(radius, length * 0.5, 0.0));

    // --- 2. Execute: Call the public conversion ---
    // This internally triggers cutWire -> fixSections -> calculateCoon -> toFace
    TopoDS_Face result = cocoon::convert(boundaryLoop);

    // --- 3. Verify ---
    SECTION("The face is valid and has the correct topology")
    {
        REQUIRE(!result.IsNull());
        CHECK(result.ShapeType() == TopAbs_FACE);
    }

    SECTION("The 3D surface area is correct (Halfpipe Area)")
    {
        GProp_GProps properties;
        BRepGProp::SurfaceProperties(result, properties);

        double actualArea = properties.Mass();
        // Expected Area for a half-cylinder: (2 * PI * r * h) / 2 => PI * r * h
        double expectedArea = M_PI * radius * length;

        // We use a small epsilon (0.5%) to account for the chordal error
        // of approximating a circle with 20 segments.
        REQUIRE(actualArea == Approx(expectedArea).epsilon(0.005));
    }
}

TEST_CASE("cocoon::convert - Degenerate Strip Fallback", "[cocoon]")
{
    // Create a "strip" so thin that Coons/BSpline might struggle,
    // forcing the fallback logic.
    std::vector<glm::dvec3> thinWire = {
        {  0,      0, 0 },
        { 10,      0, 0 },
        { 10, 0.0001, 0 },
        {  0, 0.0001, 0 }
    };

    TopoDS_Face result = cocoon::convert(thinWire);

    REQUIRE(!result.IsNull());

    GProp_GProps properties;
    BRepGProp::SurfaceProperties(result, properties);

    // Area should be 10 * 0.0001 = 0.001
    CHECK(properties.Mass() == Approx(0.001).margin(1e-6));
}
#endif
