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

struct CadOutlineVertexInput
{
    float3 position : POSITION;
    float3 otherPos : NORMAL;
    float3 sideInfo : COLOR;
};

struct CadOutlineVertexOutput
{
    float4 position : SV_Position;
    float4 color    : COLOR0;
};

[shader("vertex")]
CadOutlineVertexOutput main(CadOutlineVertexInput input)
{
    CadOutlineVertexOutput output;

    float4 clip0 = mul(scene.projectionMatrix, mul(scene.viewMatrix, mul(outlineParams.modelMatrix, float4(input.position, 1.0))));
    float4 clip1 = mul(scene.projectionMatrix, mul(scene.viewMatrix, mul(outlineParams.modelMatrix, float4(input.otherPos, 1.0))));

    float2 ndc0 = clip0.xy / max(abs(clip0.w), 1e-5);
    float2 ndc1 = clip1.xy / max(abs(clip1.w), 1e-5);

    float2 viewport    = max(outlineParams.viewportSize, float2(1.0, 1.0));
    float2 screenDelta = (ndc1 - ndc0) * 0.5 * viewport;
    float  len         = length(screenDelta);
    float2 dir         = (len > 1e-5) ? (screenDelta / len) : float2(1.0, 0.0);
    float2 perp        = float2(-dir.y, dir.x);

    float  halfWidth = 0.5 * (outlineParams.lineColor.w > 0.0 ? outlineParams.lineColor.w : 2.0);
    float2 offsetNdc = perp * (input.sideInfo.x * halfWidth) * (2.0 / viewport);

    clip0.xy += offsetNdc * clip0.w;
    clip0.z  -= 0.0002 * clip0.w;

    output.position = clip0;
    output.color    = float4(outlineParams.lineColor.rgb, 1.0);
    return output;
}
