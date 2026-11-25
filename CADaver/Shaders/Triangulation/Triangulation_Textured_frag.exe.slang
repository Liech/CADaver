import Triangulation.TriangulationCore;
import Triangulation.TexturedSurfaceModifier;

[shader("fragment")]
RasterizerOutput main(TriangulationVertexOutput input)
{
    TexturedSurfaceModifier modifier;
    return shadeSurface(input, modifier);
}
