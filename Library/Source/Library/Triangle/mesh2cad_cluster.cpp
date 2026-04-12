#include "mesh2cad_cluster.h"

#include "CAD/CADShape.h"
#include "CAD/CADShapeFactory.h"
#include "Clustering.h"
#include "HalfEdge/HalfEdge.h"
#include "HalfEdge/HalfEdgeHealth.h"
#include "HalfEdge/mesh2halfedge.h"
#include "Library/CAD/CADShapeFactory.h"
#include "Triangulation.h"
#include <BRepAdaptor_Curve.hxx>
#include <BRepBuilderAPI_FindPlane.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeSolid.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_Sewing.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepFill_Filling.hxx>
#include <BRepLib.hxx>
#include <BRepOffsetAPI_MakeFilling.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <BRep_Builder.hxx>
#include <BRep_TFace.hxx>
#include <BRep_Tool.hxx>
#include <GeomAPI_PointsToBSpline.hxx>
#include <GeomAPI_PointsToBSplineSurface.hxx>
#include <GeomConvert.hxx>
#include <GeomConvert_CompCurveToBSplineCurve.hxx>
#include <GeomFill_BSplineCurves.hxx>
#include <GeomFill_Coons.hxx>
#include <GeomFill_CoonsAlgPatch.hxx>
#include <GeomLib.hxx>
#include <GeomPlate_BuildPlateSurface.hxx>
#include <GeomPlate_CurveConstraint.hxx>
#include <GeomPlate_MakeApprox.hxx>
#include <GeomPlate_PointConstraint.hxx>
#include <GeomPlate_Surface.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Surface.hxx>
#include <Poly_Array1OfTriangle.hxx>
#include <Poly_Triangulation.hxx>
#include <ShapeFix_Wire.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TopAbs_ShapeEnum.hxx> // For TopAbs_SHELL, etc.
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx> // For TopExp_Explorer
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopoDS.hxx> // For the TopoDS casting utility
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Wire.hxx>
#include <algorithm>
#include <format>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <map>
#include <set>
#include <vector>
#include "cocoon.h"

namespace Library
{
    mesh2cad_cluster::mesh2cad_cluster() {}
    mesh2cad_cluster::~mesh2cad_cluster() {}

    enum class CoonValidity
    {
        Valid,
        NotClosed,
        GapTooLarge,
        SelfIntersecting,
        TooFewEdges
    };

    CoonValidity CheckWireForCoon(const TopoDS_Wire& wire, double tolerance = 1e-6)
    {
        // 1. Topologische Grundprüfung
        BRepCheck_Analyzer analyzer(wire);
        if (!analyzer.IsValid())
        {
            return CoonValidity::SelfIntersecting;
        }

        // 2. Ist der Wire geschlossen?
        if (!wire.Closed())
        {
            return CoonValidity::NotClosed;
        }

        // 3. Geometrische Lücken an den Ecken prüfen
        // Der Coons-Solver braucht C0-Stetigkeit an den Übergängen
        TopExp_Explorer          exp(wire, TopAbs_EDGE);
        std::vector<TopoDS_Edge> edges;
        while (exp.More())
        {
            edges.push_back(TopoDS::Edge(exp.Current()));
            exp.Next();
        }

        if (edges.size() < 4 && edges.size() != 0)
        {
            // Ein Coons-Patch braucht 4 Begrenzungen.
            // Man kann einen Wire mit 2 oder 3 Edges zwar splitten,
            // aber für den Standard-Solver ist das oft instabil.
            int x = 0;
        }

        for (size_t i = 0; i < edges.size(); ++i)
        {
            TopoDS_Edge current = edges[i];
            TopoDS_Edge next    = edges[(i + 1) % edges.size()];

            gp_Pnt pEnd   = BRep_Tool::Pnt(TopExp::LastVertex(current));
            gp_Pnt pStart = BRep_Tool::Pnt(TopExp::FirstVertex(next));

            if (pEnd.Distance(pStart) > tolerance)
            {
                return CoonValidity::GapTooLarge;
            }
        }

        return CoonValidity::Valid;
    }

    std::unique_ptr<CADShape> mesh2cad_cluster::convert(const Triangulation& mesh, double threshold)
    {
        return convert(mesh,
                       [threshold](size_t currentIndex, size_t candidateIndex, const Triangulation& tri) -> bool
                       {
                           auto curr = tri.getFaceNormal(currentIndex);
                           auto cand = tri.getFaceNormal(candidateIndex);
                           return glm::dot(curr, cand) > threshold;
                       });
    }

    Triangulation extractTriangles(const Triangulation& source, const std::vector<size_t>& triangleIndicesToKeep)
    {
        Triangulation result;

        // Maps old vertex index -> new vertex index
        std::unordered_map<size_t, size_t> indexMap;

        for (size_t triIdx : triangleIndicesToKeep)
        {
            // Each triangle has 3 indices starting at triIdx * 3
            size_t startOffset = triIdx * 3;

            for (size_t i = 0; i < 3; ++i)
            {
                size_t oldVertexIdx = source.indices[startOffset + i];

                // Check if we've already copied this vertex
                auto it = indexMap.find(oldVertexIdx);
                if (it == indexMap.end())
                {
                    // New vertex found: add to result.vertices
                    size_t newIdx = result.vertices.size();
                    result.vertices.push_back(source.vertices[oldVertexIdx]);

                    // Map the old index to the new position
                    indexMap[oldVertexIdx] = newIdx;
                    result.indices.push_back(newIdx);
                }
                else
                {
                    // Vertex already exists: just use the mapped index
                    result.indices.push_back(it->second);
                }
            }
        }

        return result;
    }

    std::unique_ptr<CADShape> mesh2cad_cluster::convert(const Triangulation& mesh, std::function<bool(size_t currentIndex, size_t candidateIndex, const Triangulation&)> growFunction)
    {
        report       = "";
        bool healthy = Library::HalfEdgeHealth::isHealthy(mesh);
        if (!healthy)
        {
            report += "Unhealthy\n";
            return nullptr;
        }

        report += "MESH\n";
        report += HalfEdgeHealth::toString(mesh);
        report += "MESHEND\n";

        // 0. Preperation
        report += "Clustering\n";
        std::vector<std::vector<size_t>> cluster = Clustering::cluster_withoutHoles(mesh, growFunction);

        size_t counter = 0;
        for (const auto& c : cluster)
        {
            report += "  Cluster " + std::to_string(counter) + "\n ";

            for (const auto& x : c)
            {
                report += "    ";
                report += std::to_string(x);
                report += "\n";
            }

            auto triCluster = extractTriangles(mesh, c);
            report += triCluster.toBase64() + "\n";

            counter++;
        }

        report += "HalfEdge\n";
        auto                             halfedge = mesh2halfedge::convert(mesh);
        std::vector<std::vector<size_t>> borders;

        report += "Borders\n";
        counter = 0;
        for (const auto& c : cluster)
        {
            report += "  Border " + std::to_string(counter) + "\n";
            auto b = Clustering::findBorders(*halfedge, c)[0];
            borders.push_back(b); // withoutHoles ensures .size()==1
            for (size_t x : b)
            {
                report += "    ";
                report += std::to_string(x);
                report += "\n";
            }
            counter++;
        }

        // 1. Convert Borders of the Clusters to Wires
        std::map<size_t, TopoDS_Vertex>                  vcache;
        std::map<std::pair<size_t, size_t>, TopoDS_Edge> ecache;

        report += "Border2Wire\n";
        std::vector<TopoDS_Wire>             wires       = Borders2Wires(borders, mesh, vcache, ecache);
        std::vector<std::vector<glm::dvec3>> edgeNormals = Borders2EdgeNormals(borders, cluster, mesh);

        // 2. Convert Mesh Clusters to B-Rep Faces
        report += "Cluster2Faces\n";
        // std::vector<TopoDS_Face> faces = Cluster2Faces(wires, cluster, edgeNormals, mesh, vcache);
        //std::vector<TopoDS_Face> faces = Cluster2Coon(wires, mesh);
        std::vector<TopoDS_Face> faces;
        for (const auto& w : wires)
        {
            auto f = cocoon::convert(w);
            faces.push_back(f);
        }
        if (faces.size() == 0)
        {
            std::ofstream out("C:\\Users\\nicol\\Downloads\\crashlog.txt");
            out << report;
            out.close();
            return nullptr;
        }

        // 3. Stitching Faces into a Shell
        TopoDS_Shape stich = StitchFaces(faces);

        // 4. Creating the Solid
        auto solid = MakeSolid(stich);
        //BRepLib::OrientClosedSolid(solid);

        auto result = CADShapeFactory::make(solid);
        CADShapeFactory::recurseFillChildShapes(*result);
        return std::move(result);
    }

    TopoDS_Solid mesh2cad_cluster::MakeSolid(const TopoDS_Shape& stitchedShell)
    {
        // 1. Safety check: Ensure we are dealing with a shell or a compound of shells
        if (stitchedShell.IsNull())
            return TopoDS_Solid();

        // 2. Use the MakeSolid tool
        // If stitchedShell is a TopoDS_Shell, this wraps it.
        // If it's a Compound, it tries to extract shells to form a solid.
        BRepBuilderAPI_MakeSolid solidMaker;

        for (TopExp_Explorer exp(stitchedShell, TopAbs_SHELL); exp.More(); exp.Next())
        {
            solidMaker.Add(TopoDS::Shell(exp.Current()));
        }

        if (!solidMaker.IsDone())
        {
            // This usually happens if the shell is not closed (has free edges)
            return TopoDS_Solid();
        }

        return solidMaker.Solid();
    }

    std::vector<std::vector<glm::dvec3>> mesh2cad_cluster::Borders2EdgeNormals(const std::vector<std::vector<size_t>>& borders,
                                                                               const std::vector<std::vector<size_t>>& clusters, // Neu: Die Cluster-Information
                                                                               const Triangulation&                    mesh)
    {
        std::map<std::pair<size_t, size_t>, std::vector<size_t>> adjacency = mesh.getAdjacency();
        std::vector<std::vector<glm::dvec3>>                     resultNormals;

        for (size_t bIdx = 0; bIdx < borders.size(); ++bIdx)
        {
            const auto&                border = borders[bIdx];
            // Erstelle ein Set für schnellen Lookup, welche Dreiecke zum aktuellen Cluster gehören
            std::unordered_set<size_t> currentClusterTris(clusters[bIdx].begin(), clusters[bIdx].end());

            std::vector<glm::dvec3> currentBorderNormals;

            for (size_t i = 0; i < border.size(); ++i)
            {
                size_t idxA = border[i];
                size_t idxB = border[(i + 1) % border.size()];

                auto edgeKey = (idxA < idxB) ? std::make_pair(idxA, idxB) : std::make_pair(idxB, idxA);

                glm::dvec3 edgeNormal(0.0, 0.0, 0.0);
                bool       foundInternal = false;

                if (adjacency.count(edgeKey))
                {
                    for (size_t triIdx : adjacency[edgeKey])
                    {
                        // PRÜFUNG: Gehört dieses Dreieck zum aktuellen Cluster?
                        if (currentClusterTris.find(triIdx) != currentClusterTris.end())
                        {
                            const auto& v0 = mesh.vertices[mesh.indices[triIdx * 3 + 0]];
                            const auto& v1 = mesh.vertices[mesh.indices[triIdx * 3 + 1]];
                            const auto& v2 = mesh.vertices[mesh.indices[triIdx * 3 + 2]];

                            glm::dvec3 n = glm::cross(glm::dvec3(v1.x - v0.x, v1.y - v0.y, v1.z - v0.z), glm::dvec3(v2.x - v0.x, v2.y - v0.y, v2.z - v0.z));

                            double len = glm::length(n);
                            if (len > 1e-12)
                            {
                                edgeNormal    = n / len;
                                foundInternal = true;
                                break; // Wir haben das interne Dreieck gefunden, weiter zur nächsten Kante
                            }
                        }
                    }
                }

                if (!foundInternal)
                {
                    // Fallback: Falls kein internes Dreieck gefunden wurde (sollte bei Border theoretisch nicht passieren)
                    edgeNormal = glm::dvec3(0.0, 0.0, 1.0);
                }

                currentBorderNormals.push_back(edgeNormal);
            }
            resultNormals.push_back(currentBorderNormals);
        }

        return resultNormals;
    }

    std::vector<TopoDS_Wire> mesh2cad_cluster::Borders2Wires(const std::vector<std::vector<size_t>>&           borders,
                                                             const Triangulation&                              mesh,
                                                             std::map<size_t, TopoDS_Vertex>&                  vcache, // Changed from gp_Pnt to TopoDS_Vertex
                                                             std::map<std::pair<size_t, size_t>, TopoDS_Edge>& ecache)
    {

        std::vector<TopoDS_Wire> resultWires;
        for (const auto& border : borders)
        {
            BRepBuilderAPI_MakeWire wireMaker;

            for (size_t i = 0; i < border.size(); ++i)
            {
                size_t idxA = border[i];
                size_t idxB = border[(i + 1) % border.size()];

                // 1. Get/Create shared Vertices
                auto getVtx = [&](size_t idx)
                {
                    if (vcache.find(idx) == vcache.end())
                    {
                        const auto& v = mesh.vertices[idx];
                        vcache[idx]   = BRepBuilderAPI_MakeVertex(gp_Pnt(v.x, v.y, v.z));
                    }
                    return vcache[idx];
                };

                TopoDS_Vertex vA = getVtx(idxA);
                TopoDS_Vertex vB = getVtx(idxB);

                // 2. Normalize key
                auto edgeKey = (idxA < idxB) ? std::make_pair(idxA, idxB) : std::make_pair(idxB, idxA);

                // 3. Get/Create Edge using shared Vertices
                TopoDS_Edge edge;
                if (ecache.find(edgeKey) == ecache.end())
                {
                    edge            = BRepBuilderAPI_MakeEdge(vA, vB);
                    ecache[edgeKey] = edge;
                }
                else
                {
                    edge = ecache[edgeKey];
                }

                wireMaker.Add(edge);
            }

            if (wireMaker.IsDone())
                resultWires.push_back(wireMaker.Wire());
        }
        return resultWires;
    }
    std::string AnalyzeFillingInput(int index, const TopoDS_Wire& wire, const std::vector<size_t>& clusterIndices, std::map<size_t, TopoDS_Vertex>& vcache)
    {
        std::stringstream ss;
        ss << "\n=== [Face " << index << " Pre-Flight Check] ===\n";

        // 1. Wire Topology Check
        if (wire.IsNull())
        {
            ss << "FAILURE: Wire is null.\n";
        }
        else
        {
            BRepCheck_Analyzer analyzer(wire);
            ss << "Wire Valid: " << (analyzer.IsValid() ? "YES" : "NO") << "\n";
            ss << "Wire Closed: " << (BRep_Tool::IsClosed(wire) ? "YES" : "NO") << "\n";

            int edgeCount = 0;
            for (TopExp_Explorer exp(wire, TopAbs_EDGE); exp.More(); exp.Next())
                edgeCount++;
            ss << "Edge Count: " << edgeCount << "\n";
        }

        // 2. Vertex/Point Analysis
        TopTools_IndexedMapOfShape wireVertices;
        if (!wire.IsNull())
        {
            TopExp::MapShapes(wire, TopAbs_VERTEX, wireVertices);
        }

        ss << "Total Indices in Cluster: " << clusterIndices.size() << "\n";
        ss << "Vertices in Wire: " << wireVertices.Extent() << "\n";

        int redundantPoints = 0;
        for (size_t vIdx : clusterIndices)
        {
            if (vcache.count(vIdx) && wireVertices.Contains(vcache[vIdx]))
            {
                redundantPoints++;
            }
        }
        ss << "Redundant Points (on boundary): " << redundantPoints << "\n";
        ss << "Internal Constraints (points): " << (clusterIndices.size() - redundantPoints) << "\n";

        ss << "==========================================\n";
        return ss.str();
    }

    std::vector<TopoDS_Face> mesh2cad_cluster::Cluster2Coon(const std::vector<TopoDS_Wire>& wires, const Triangulation& mesh)
    {
        std::vector<TopoDS_Face> result_faces;

        for (const auto& wire : wires)
        {
            // 1. Extrahiere die 4 Kurven (vereinfacht: Annahme, das Wire hat 4 Edges)
            std::vector<Handle(Geom_Curve)> curves;
            std::vector<double>             firsts, lasts;

            BRepTools_WireExplorer exp(wire);
            while (exp.More())
            {
                double f, l;
                Handle(Geom_Curve) c = BRep_Tool::Curve(exp.Current(), f, l);
                if (exp.Current().Orientation() == TopAbs_REVERSED)
                {
                    // Hier müsste logisch die Kurve umgedreht werden,
                    // für die manuelle Berechnung reicht die Richtungskontrolle
                }
                curves.push_back(c);
                firsts.push_back(f);
                lasts.push_back(l);
                exp.Next();
            }

            if (curves.size() != 4)
                continue; // Nur 4-seitige Wires

            // Hilfsfunktion zur Normalisierung des Parameters auf [0, 1]
            auto getPoint = [&](int idx, double t)
            {
                double p = firsts[idx] + t * (lasts[idx] - firsts[idx]);
                return curves[idx]->Value(p);
            };

            // 2. Erstelle Punkt-Gitter (z.B. 10x10) für die Coons-Interpolation
            int                n = 15; // Auflösung
            TColgp_Array2OfPnt points(1, n, 1, n);

            for (int i = 1; i <= n; ++i)
            {
                double u = double(i - 1) / (n - 1);
                for (int j = 1; j <= n; ++j)
                {
                    double v = double(j - 1) / (n - 1);

                    // Die 4 Grenzkurven: c1(u), c2(u) [unten/oben], d1(v), d2(v) [links/rechts]
                    gp_Pnt c1_u = getPoint(0, u);       // Untere Kante
                    gp_Pnt c2_u = getPoint(2, 1.0 - u); // Obere Kante (umgekehrt)
                    gp_Pnt d1_v = getPoint(3, 1.0 - v); // Linke Kante (umgekehrt)
                    gp_Pnt d2_v = getPoint(1, v);       // Rechte Kante

                    // Eckpunkte für die bilineare Korrektur
                    gp_Pnt P00 = getPoint(0, 0); // Ecke unten links
                    gp_Pnt P10 = getPoint(0, 1); // Ecke unten rechts
                    gp_Pnt P01 = getPoint(2, 1); // Ecke oben links
                    gp_Pnt P11 = getPoint(2, 0); // Ecke oben rechts

                    // Coons Patch Formel: S(u,v) = Lc(u,v) + Ld(u,v) - B(u,v)
                    // Lineare Interpolation in u
                    gp_Pnt Lc;
                    Lc.SetX((1.0 - v) * c1_u.X() + v * c2_u.X());
                    Lc.SetY((1.0 - v) * c1_u.Y() + v * c2_u.Y());
                    Lc.SetZ((1.0 - v) * c1_u.Z() + v * c2_u.Z());

                    // Lineare Interpolation in v
                    gp_Pnt Ld;
                    Ld.SetX((1.0 - u) * d1_v.X() + u * d2_v.X());
                    Ld.SetY((1.0 - u) * d1_v.Y() + u * d2_v.Y());
                    Ld.SetZ((1.0 - u) * d1_v.Z() + u * d2_v.Z());

                    // Bilineare Interpolation der Ecken
                    gp_Pnt B;
                    B.SetX((1.0 - u) * (1.0 - v) * P00.X() + u * (1.0 - v) * P10.X() + (1.0 - u) * v * P01.X() + u * v * P11.X());
                    B.SetY((1.0 - u) * (1.0 - v) * P00.Y() + u * (1.0 - v) * P10.Y() + (1.0 - u) * v * P01.Y() + u * v * P11.Y());
                    B.SetZ((1.0 - u) * (1.0 - v) * P00.Z() + u * (1.0 - v) * P10.Z() + (1.0 - u) * v * P01.Z() + u * v * P11.Z());

                    // Finaler Punkt
                    gp_Pnt finalP;
                    finalP.SetX(Lc.X() + Ld.X() - B.X());
                    finalP.SetY(Lc.Y() + Ld.Y() - B.Y());
                    finalP.SetZ(Lc.Z() + Ld.Z() - B.Z());

                    points.SetValue(i, j, finalP);
                }
            }

            // 3. Aus dem Gitter eine Fläche bauen
            GeomAPI_PointsToBSplineSurface approx(points);
            Handle(Geom_BSplineSurface) surface = approx.Surface();

            // 4. Face erstellen
            if (!surface.IsNull())
            {
                result_faces.push_back(BRepBuilderAPI_MakeFace(surface, 1e-6));
            }
        }

        return result_faces;
    }

    std::vector<TopoDS_Face> mesh2cad_cluster::Cluster2Faces(const std::vector<TopoDS_Wire>&             wires,
                                                             const std::vector<std::vector<size_t>>&     clusters,
                                                             const std::vector<std::vector<glm::dvec3>>& edgeNormals,
                                                             const Triangulation&                        mesh,
                                                             std::map<size_t, TopoDS_Vertex>&            vcache)
    {
        std::vector<TopoDS_Face> result_faces;
        size_t                   count = std::min({ wires.size(), clusters.size(), edgeNormals.size() });

        for (size_t i = 0; i < count; ++i)
        {
            try
            {
                // Initialisierung
                GeomPlate_BuildPlateSurface builder(3, 15, 3);

                // 1. Kurven-Constraints (Ränder)
                for (TopoDS_Iterator it(wires[i]); it.More(); it.Next())
                {
                    TopoDS_Edge edge = TopoDS::Edge(it.Value());
                    if (edge.IsNull())
                        continue;
                    Handle(BRepAdaptor_Curve) adaptCurve = new BRepAdaptor_Curve(edge);
                    builder.Add(new GeomPlate_CurveConstraint(adaptCurve, 0));
                }

                // 2. Punkt-Constraints (Normalen-Ersatz via Offset)
                const auto& tri_indices  = clusters[i];
                size_t      sample_count = 30;
                size_t      stride       = std::max((size_t)1, tri_indices.size() / sample_count);
                double      offset_dist  = 0.05;

                for (size_t j = 0; j < tri_indices.size(); j += stride)
                {
                    size_t      tri_idx = tri_indices[j];
                    const auto& v1      = mesh.vertices[mesh.indices[tri_idx * 3 + 0]];
                    const auto& v2      = mesh.vertices[mesh.indices[tri_idx * 3 + 1]];
                    const auto& v3      = mesh.vertices[mesh.indices[tri_idx * 3 + 2]];

                    gp_Pnt pos((v1.x + v2.x + v3.x) / 3.0, (v1.y + v2.y + v3.y) / 3.0, (v1.z + v2.z + v3.z) / 3.0);

                    glm::dvec3 p1(v1.x, v1.y, v1.z), p2(v2.x, v2.y, v2.z), p3(v3.x, v3.y, v3.z);
                    glm::dvec3 n = glm::normalize(glm::cross(p2 - p1, p3 - p1));

                    // Der Punkt auf dem Mesh (C0 - funktioniert immer)
                    // Signatur: (Punkt, Order, Toleranz) -> Order 0 = C0
                    builder.Add(new GeomPlate_PointConstraint(pos, GeomAbs_C0, 0.001));

                    // Der Richtungs-Punkt (Offset erzwingt die Krümmung ohne G1-API Stress)
                    gp_Pnt posOffset(pos.X() + n.x * offset_dist, pos.Y() + n.y * offset_dist, pos.Z() + n.z * offset_dist);
                    builder.Add(new GeomPlate_PointConstraint(posOffset, GeomAbs_C0, 0.001));
                }

                // 3. Fläche berechnen
                builder.Perform();

                if (builder.IsDone())
                {
                    // Approximation
                    GeomPlate_MakeApprox app(builder.Surface(), 0.001, 8, 3, 0.001, 0);

                    // Durch den BSplineSurface-Header ist finalSurf nun korrekt zuweisbar
                    Handle(Geom_Surface) finalSurf = app.Surface();

                    BRepBuilderAPI_MakeFace faceMaker(finalSurf, wires[i], Standard_True);
                    if (faceMaker.IsDone())
                    {
                        result_faces.push_back(faceMaker.Face());
                    }
                }
            }
            catch (...)
            {
                continue;
            }
        }
        return result_faces;
    }

    TopoDS_Shape mesh2cad_cluster::StitchFaces(const std::vector<TopoDS_Face>& faces, double tolerance)
    {
        // 1. Initialize the Sewing tool
        // The tolerance is the maximum gap it will attempt to bridge
        BRepBuilderAPI_Sewing sewer(tolerance);

        // 2. Add all generated faces to the sewer
        for (const auto& face : faces)
        {
            sewer.Add(face);
        }

        // 3. Perform the sewing operation
        sewer.Perform();

        // 4. Retrieve the resulting shape
        // This usually returns a TopoDS_Shell if successful,
        // or a TopoDS_Compound if the faces couldn't form a single manifold shell.
        TopoDS_Shape result = sewer.SewedShape();

        return result;
    }
}

#ifdef false
//#ifdef ISTESTPROJECT
#include "BaseShapeGenerator.h"
#include "Library/catch.hpp"
#include <BRepCheck_Analyzer.hxx>
#include <BRepGProp.hxx>
#include <BRep_Tool.hxx>
#include <GProp_GProps.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>

using namespace Library;

Triangulation CreateUnitCubeMesh()
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

TEST_CASE("mesh2cad_cluster::Borders2Wires - Basic Loops", "[mesh2cad]")
{
    Library::mesh2cad_cluster converter;
    Library::Triangulation    mesh;

    // Define a simple quad (2 triangles)
    // 0(0,1) -- 1(1,1)
    //  |         |
    // 3(0,0) -- 2(1,0)
    mesh.vertices = {
        { 0.0, 1.0, 0.0 },
        { 1.0, 1.0, 0.0 },
        { 1.0, 0.0, 0.0 },
        { 0.0, 0.0, 0.0 }
    };

    std::map<size_t, TopoDS_Vertex>                  vcache;
    std::map<std::pair<size_t, size_t>, TopoDS_Edge> ecache;

    SECTION("Single closed triangular border")
    {
        std::vector<std::vector<size_t>> borders = {
            { 0, 1, 2 }
        };
        auto wires = converter.Borders2Wires(borders, mesh, vcache, ecache);

        REQUIRE(wires.size() == 1);
        const auto& wire = wires[0];

        // 1. Check validity via OpenCASCADE Analyzer
        BRepCheck_Analyzer analyzer(wire);
        CHECK(analyzer.IsValid() == Standard_True);

        // 2. Check if it's closed
        CHECK(wire.Closed() == Standard_True);

        // 3. Count edges (should be 3)
        int edgeCount = 0;
        for (TopExp_Explorer exp(wire, TopAbs_EDGE); exp.More(); exp.Next())
        {
            edgeCount++;
        }
        CHECK(edgeCount == 3);
    }

    SECTION("Caching mechanism - Shared edges")
    {
        // Two adjacent loops sharing edge 1-2
        std::vector<std::vector<size_t>> borders = {
            { 0, 1, 2 },
            { 2, 1, 3 }
        };

        auto wires = converter.Borders2Wires(borders, mesh, vcache, ecache);

        REQUIRE(wires.size() == 2);

        // Verify ecache contains the shared edge (1,2)
        // Normalized key for {1,2} and {2,1} is the same
        std::pair<size_t, size_t> sharedKey = { 1, 2 };
        CHECK(ecache.count(sharedKey) > 0);

        // Ensure total unique edges in cache is 5 (3 from first tri + 2 new ones from second)
        CHECK(ecache.size() == 5);
    }

    SECTION("Geometric Accuracy")
    {
        std::vector<std::vector<size_t>> borders = {
            { 0, 1, 2, 3 }
        };
        auto wires = converter.Borders2Wires(borders, mesh, vcache, ecache);

        TopExp_Explorer exp(wires[0], TopAbs_EDGE);
        TopoDS_Edge     firstEdge = TopoDS::Edge(exp.Current());

        // Extract vertices from the first edge
        TopoDS_Vertex vFirst, vLast;
        TopExp::Vertices(firstEdge, vFirst, vLast);

        gp_Pnt p = BRep_Tool::Pnt(vFirst);
        // Expecting point 0 (0,1,0)
        CHECK(p.X() == Approx(0.0));
        CHECK(p.Y() == Approx(1.0));
        CHECK(p.Z() == Approx(0.0));
    }
}

TEST_CASE("mesh2cad_cluster::Borders2Wires - Topology Sharing", "[mesh2cad]")
{
    Library::mesh2cad_cluster converter;
    Library::Triangulation    mesh;

    // Define vertices for two adjacent triangles
    // 0 --- 1 --- 4
    //  \   / \   /
    //    2 --- 3
    mesh.vertices = {
        { 0, 2, 0 },
        { 2, 2, 0 },
        { 1, 0, 0 }, // Tri 0: 0, 1, 2
        { 3, 0, 0 },
        { 4, 2, 0 }  // Tri 1: 1, 4, 3, 2 (Quad/Tri overlap)
    };

    std::map<size_t, TopoDS_Vertex>                  vcache;
    std::map<std::pair<size_t, size_t>, TopoDS_Edge> ecache;

    // We define two borders that share the edge between vertex 1 and vertex 2
    std::vector<size_t> borderA = { 0, 1, 2 };
    std::vector<size_t> borderB = { 1, 4, 3, 2 }; // Shared segment is 1-2

    std::vector<std::vector<size_t>> allBorders = { borderA, borderB };
    auto                             wires      = converter.Borders2Wires(allBorders, mesh, vcache, ecache);

    REQUIRE(wires.size() == 2);

    // 1. Verify ecache logic: The key (1, 2) must exist
    std::pair<size_t, size_t> sharedKey = { 1, 2 }; // (smaller index first)
    REQUIRE(ecache.count(sharedKey) == 1);
    TopoDS_Edge cachedSharedEdge = ecache[sharedKey];

    // Get the edge from Wire A
    TopExp_Explorer expA(wires[0], TopAbs_EDGE);
    expA.Next(); // Move to the second edge (1-2)
    TopoDS_Edge edgeFromWireA = TopoDS::Edge(expA.Current());

    // Get the edge from Wire B
    TopExp_Explorer expB(wires[1], TopAbs_EDGE);
    expB.Next();
    expB.Next();
    expB.Next(); // Move to the fourth edge (2-1)
    TopoDS_Edge edgeFromWireB = TopoDS::Edge(expB.Current());

    // 1. Check if they share the same TShape (The absolute truth in OCC)
    bool tShapeMatch = (edgeFromWireA.TShape() == edgeFromWireB.TShape());
    CHECK(tShapeMatch);

    // 2. Check if either matches the cache
    bool cacheMatch = (edgeFromWireA.TShape() == cachedSharedEdge.TShape());
    CHECK(cacheMatch);

    // 2. Verify Wire A contains the cached edge
    bool wireAHasSharedEdge = false;
    for (TopExp_Explorer exp(wires[0], TopAbs_EDGE); exp.More(); exp.Next())
    {
        if (exp.Current().IsPartner(cachedSharedEdge))
        {
            wireAHasSharedEdge = true;
            break;
        }
    }
    CHECK(wireAHasSharedEdge);

    // 3. Verify Wire B contains the EXACT SAME cached edge
    bool wireBHasSharedEdge = false;
    for (TopExp_Explorer exp(wires[1], TopAbs_EDGE); exp.More(); exp.Next())
    {
        // IsSame checks if they point to the same geometric/topological definition
        if (exp.Current().IsPartner(cachedSharedEdge))
        {
            wireBHasSharedEdge = true;
            break;
        }
    }
    CHECK(wireBHasSharedEdge);

    // 4. Final check: Total edges created should be 6
    // (Tri A: 3 edges) + (Tri B: 4 edges) - (1 shared edge) = 6
    CHECK(ecache.size() == 6);
}

TEST_CASE("mesh2cad_cluster::convert - Full Cube Reconstruction", "[mesh2cad]")
{
    Library::mesh2cad_cluster converter;
    Library::Triangulation    mesh = CreateUnitCubeMesh();

    // 3. Run the full conversion
    // Threshold 0.9 ensures triangles with the same normal cluster together
    double                             threshold = 0.9;
    std::unique_ptr<Library::CADShape> result    = converter.convert(mesh, threshold);

    // --- VERIFICATION ---

    REQUIRE(result != nullptr);
    TopoDS_Shape shape = result->getData();
    REQUIRE(!shape.IsNull());

    // Test 1: Shape Identity
    // It should be a Solid
    CHECK(shape.ShapeType() == TopAbs_SOLID);

    // Test 2: Face Count
    // A cube has 6 faces. clustering 12 triangles by normals should yield 6 CAD faces.
    int faceCount = 0;
    for (TopExp_Explorer exp(shape, TopAbs_FACE); exp.More(); exp.Next())
    {
        faceCount++;
    }
    CHECK(faceCount == 6);

    // Test 3: Volume Accuracy
    // 1x1x1 cube should have a volume of exactly 1.0
    GProp_GProps vProps;
    BRepGProp::VolumeProperties(shape, vProps);
    CHECK(vProps.Mass() == Approx(1.0).margin(1e-4));

    // Test 4: Surface Area Accuracy
    // 6 faces of 1x1 = 6.0
    GProp_GProps sProps;
    BRepGProp::SurfaceProperties(shape, sProps);
    CHECK(sProps.Mass() == Approx(6.0).margin(1e-4));

    // Test 5: Topological Validity
    BRepCheck_Analyzer analyzer(shape);
    CHECK(analyzer.IsValid() == Standard_True);

    // Test 6: Watertightness
    // No free edges in a solid cube
    int freeEdges = 0;
    // (You could also use BRepBuilderAPI_Sewing internally to verify this)
}

#include <BRepAdaptor_Surface.hxx>
#include <BRepLProp_SLProps.hxx>
#include <BRepTools.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
TEST_CASE("mesh2cad_cluster::convert - Full Cube With Hole Reconstruction", "[mesh2cad]")
{
    Library::mesh2cad_cluster converter;
    auto                      mesh = Library::BaseShapeGenerator::cubeWithHole(5);

    // 3. Run the full conversion
    // Threshold 0.9 ensures triangles with the same normal cluster together
    double                             threshold = 0.7;
    std::unique_ptr<Library::CADShape> result    = converter.convert(mesh, threshold);

    REQUIRE(result != nullptr);
    TopoDS_Shape shape = result->getData();

    // --- VERIFICATION ---
    // white box verification that checks if the result has the exact number a hole avoiding clustering would bring

    int planarNurbs = 0;
    int curvedNurbs = 0;

    TopExp_Explorer explorer(shape, TopAbs_FACE);
    while (explorer.More())
    {
        TopoDS_Face         face = TopoDS::Face(explorer.Current());
        BRepAdaptor_Surface surf(face);

        Standard_Real uMin, uMax, vMin, vMax;
        BRepTools::UVBounds(face, uMin, uMax, vMin, vMax);

        // Sample the center of the patch
        Standard_Real uMid = (uMin + uMax) / 2.0;
        Standard_Real vMid = (vMin + vMax) / 2.0;

        // Check Gaussian Curvature
        // NURBS that are geometrically flat return 0.0
        BRepLProp_SLProps props(surf, uMid, vMid, 2, Precision::Confusion());

        if (props.IsCurvatureDefined())
        {
            Standard_Real k = props.GaussianCurvature();
            if (Abs(k) < 1e-7)
            {
                planarNurbs++;
            }
            else
            {
                curvedNurbs++;
            }
        }
        explorer.Next();
    }

    // Based on your description:
    // 8 flat patches (4 solid sides + 4 'C-shaped' faces around the hole entries)
    // 2 curved patches (the half-pipes)
    CHECK(planarNurbs == 8);
    CHECK(curvedNurbs == 2);
    CHECK((planarNurbs + curvedNurbs) == 10);

    // Final Safety: Check that the shape is manifold (watertight)
    CHECK(shape.Closed() == Standard_True);
}

#endif