import Triangulation.TriangulationCore;
import Triangulation.DefaultSurfaceModifier;

[shader("fragment")]
RasterizerOutput main(TriangulationVertexOutput input)
{
    DefaultSurfaceModifier modifier;
    return shadeSurface(input, modifier);
}
