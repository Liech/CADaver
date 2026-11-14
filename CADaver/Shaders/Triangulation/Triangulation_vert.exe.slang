import Triangulation.TriangulationCore;

[shader("vertex")]
TriangulationVertexOutput main(TriangulationVertexInput input, uint vertexID : SV_VertexID)
{
    return buildVertexOutput(input, vertexID);
}
