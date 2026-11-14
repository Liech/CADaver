module IndexedFaceColorModifier;

import Triangulation.SurfaceModifier;

[[vk::binding(0, 1)]] StructuredBuffer<uint>   faceValueIndices;
[[vk::binding(1, 1)]] StructuredBuffer<float4> paletteColors;

public struct IndexedFaceColorModifier : ISurfaceModifier
{
    public float3 evaluateAlbedo(SurfaceContext ctx)
    {
        uint valueIndex = faceValueIndices[ctx.faceID];
        return paletteColors[valueIndex].rgb;
    }
};
