module TriangulationCore;

import Render.Scene;
import Triangulation.SurfaceModifier;

[[vk::binding(0, 0)]] public ConstantBuffer<Render.Scene> scene;

public struct MeshParams
{
    public float4   wireframeColor;
    public int      wireframeMode;
    public float    padding0;
    public float    padding1;
    public float    padding2;
    public float4x4 modelMatrix;
    public float4x4 normalMatrix;
};

[[vk::binding(1, 0)]] public ConstantBuffer<MeshParams> meshParams;

public struct TriangulationVertexInput
{
    public float3 position : POSITION;
    public float3 normal   : NORMAL;
    public float3 color    : COLOR;
};

public struct TriangulationVertexOutput
{
    public float4 position    : SV_Position;
    public float4 color       : COLOR0;
    public float3 normalView  : TEXCOORD0;
    public float3 posWorld    : TEXCOORD1;
    public float3 barycentric : COLOR1;
    public nointerpolation uint faceID   : TEXCOORD2;
    public nointerpolation uint vertexID : TEXCOORD3;
};

public struct RasterizerOutput
{
    public float4 color  : SV_Target0;
    public float4 normal : SV_Target1;
};

public TriangulationVertexOutput buildVertexOutput(TriangulationVertexInput input, uint vertexID)
{
    TriangulationVertexOutput output;

    float4 worldPos = mul(meshParams.modelMatrix, float4(input.position, 1.0));
    float4 viewPos  = mul(scene.viewMatrix, worldPos);

    output.position = mul(scene.projectionMatrix, viewPos);
    if (meshParams.wireframeMode < 0)
    {
        output.position.w += 0.00001 * meshParams.wireframeColor.x;
    }

    output.color      = float4(input.color, 1.0);
    output.normalView = normalize(mul((float3x3)scene.viewMatrix, mul((float3x3)meshParams.normalMatrix, input.normal)));
    output.posWorld   = worldPos.xyz;

    uint corner        = vertexID % 3u;
    output.barycentric = (corner == 0u) ? float3(1.0, 0.0, 0.0) : ((corner == 1u) ? float3(0.0, 1.0, 0.0) : float3(0.0, 0.0, 1.0));
    output.faceID      = vertexID / 3u;
    output.vertexID    = vertexID;

    return output;
}

public RasterizerOutput shadeSurface<TModifier : ISurfaceModifier>(TriangulationVertexOutput input, TModifier modifier)
{
    RasterizerOutput output;

    float3 normalView = normalize(input.normalView);

    SurfaceContext ctx;
    ctx.faceID     = input.faceID;
    ctx.vertexID   = input.vertexID;
    ctx.posWorld   = input.posWorld;
    ctx.normalView = normalView;
    ctx.baseColor  = input.color.rgb;

    float3 albedo = modifier.evaluateAlbedo(ctx);

    float3 lightDirView = normalize(float3(0.3, 0.8, 0.5));
    float  diff         = max(dot(normalView, lightDirView), 0.0);
    float  ambient      = 0.25;
    float  lighting     = ambient + 0.75 * diff;

    // Keep Set 0 binding 0 (scene) active in fragment SPIR-V reflection.
    if (scene.projectionMatrix[3][3] > 1e30)
    {
        albedo += float3(0.00001);
    }

    float4 surfaceColor = float4(albedo * lighting, 1.0);

    if (meshParams.wireframeMode == 1 || meshParams.wireframeMode == 2)
    {
        float3 bary       = input.barycentric;
        float3 deltas     = fwidth(bary);
        float  lineWidth  = meshParams.wireframeColor.w > 0.0 ? meshParams.wireframeColor.w : 1.5;
        float3 smoothing  = max(deltas * lineWidth, float3(1e-4));
        float3 edge       = smoothstep(float3(0.0), smoothing, bary);
        float  minEdge    = min(min(edge.x, edge.y), edge.z);
        float  wireFactor = 1.0 - minEdge;

        if (meshParams.wireframeMode == 2)
        {
            if (wireFactor < 0.1)
            {
                discard;
            }
            output.color = float4(meshParams.wireframeColor.rgb, 1.0);
        }
        else
        {
            output.color = float4(lerp(surfaceColor.rgb, meshParams.wireframeColor.rgb, wireFactor), 1.0);
        }
    }
    else
    {
        output.color = surfaceColor;
    }

    output.normal = float4(normalView * 0.5 + 0.5, 1.0);

    return output;
}
