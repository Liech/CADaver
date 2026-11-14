import Triangulation.TriangulationCore;
import Triangulation.IndexedFaceColorModifier;

[shader("fragment")]
RasterizerOutput main(TriangulationVertexOutput input)
{
    IndexedFaceColorModifier modifier;
    return shadeSurface(input, modifier);
}
