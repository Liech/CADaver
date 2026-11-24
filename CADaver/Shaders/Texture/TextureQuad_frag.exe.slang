import Render.Scene;

[[vk::binding(0, 0)]] ConstantBuffer<Render.Scene> scene;

struct QuadParams
{
    float4   tintColor;
    int      selected;
    float    padding0;
    float    padding1;
    float    padding2;
    float4x4 modelMatrix;
    float4x4 normalMatrix;
};

[[vk::binding(1, 0)]] ConstantBuffer<QuadParams> quadParams;
[[vk::binding(0, 1)]] Sampler2D                  colorTexture;

struct VertexOutput
{
    float4 position   : SV_Position;
    float2 uv         : TEXCOORD0;
    float3 normalView : TEXCOORD1;
};

struct RasterizerOutput
{
    float4 color  : SV_Target0;
    float4 normal : SV_Target1;
};

[shader("fragment")]
RasterizerOutput main(VertexOutput input)
{
    RasterizerOutput output;

    float4 texColor = colorTexture.Sample(input.uv);
    if (texColor.a < 0.01)
    {
        discard;
    }

    float3 rgb = texColor.rgb;
    if (quadParams.selected != 0)
    {
        rgb = lerp(rgb, quadParams.tintColor.rgb, 0.25);
    }

    if (scene.projectionMatrix[3][3] > 1e30)
    {
        rgb += float3(0.00001);
    }

    float3 normalView = normalize(input.normalView);
    output.color      = float4(rgb, texColor.a);
    output.normal     = float4(normalView * 0.5 + 0.5, 1.0);

    return output;
}
