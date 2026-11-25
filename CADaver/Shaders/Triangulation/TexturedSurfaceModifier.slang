module TexturedSurfaceModifier;

import Triangulation.SurfaceModifier;

[[vk::binding(0, 1)]] StructuredBuffer<float2> cornerUVs;
[[vk::binding(1, 1)]] Sampler2D                colorTexture;

public struct TexturedSurfaceModifier : ISurfaceModifier
{
    public float3 evaluateAlbedo(SurfaceContext ctx)
    {
        uint   base = ctx.faceID * 3u;
        float2 uv0  = cornerUVs[base];
        float2 uv1  = cornerUVs[base + 1u];
        float2 uv2  = cornerUVs[base + 2u];
        float2 uv   = uv0 * ctx.barycentric.x + uv1 * ctx.barycentric.y + uv2 * ctx.barycentric.z;
        return colorTexture.Sample(uv).rgb;
    }
};
