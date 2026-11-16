import Render.Scene;

[[vk::binding(0, 0)]] ConstantBuffer<Render.Scene> scene;

struct CadOutlineParams
{
    float4 lineColor;
    float2 viewportSize;
    float2 padding;
    float4x4 modelMatrix;
};

[[vk::binding(1, 0)]] ConstantBuffer<CadOutlineParams> outlineParams;

struct CadOutlineVertexOutput
{
    float4 position : SV_Position;
    float4 color    : COLOR0;
};

struct RasterizerOutput
{
    float4 color  : SV_Target0;
    float4 normal : SV_Target1;
};

[shader("fragment")]
RasterizerOutput main(CadOutlineVertexOutput input)
{
    RasterizerOutput output;
    float3           col = input.color.rgb;

    if (scene.projectionMatrix[3][3] > 1e30 || outlineParams.viewportSize.x < -1e30)
    {
        col += float3(0.00001);
    }

    output.color  = float4(col, 1.0);
    output.normal = float4(0.5, 0.5, 1.0, 1.0);
    return output;
}
