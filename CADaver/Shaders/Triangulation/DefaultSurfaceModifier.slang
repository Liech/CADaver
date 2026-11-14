module DefaultSurfaceModifier;

import Triangulation.SurfaceModifier;

public struct DefaultSurfaceModifier : ISurfaceModifier
{
    public float3 evaluateAlbedo(SurfaceContext ctx)
    {
        return ctx.baseColor;
    }
};
