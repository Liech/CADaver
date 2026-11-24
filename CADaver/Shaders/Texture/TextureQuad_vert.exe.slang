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

struct VertexInput
{
    float3 position : POSITION;
    float3 normal   : NORMAL;
    float3 uvData   : COLOR;
};

struct VertexOutput
{
    float4 position   : SV_Position;
    float2 uv         : TEXCOORD0;
    float3 normalView : TEXCOORD1;
};

[shader("vertex")]
VertexOutput main(VertexInput input)
{
    VertexOutput output;

    float4 worldPos   = mul(quadParams.modelMatrix, float4(input.position, 1.0));
    float4 viewPos    = mul(scene.viewMatrix, worldPos);
    output.position   = mul(scene.projectionMatrix, viewPos);
    output.uv         = input.uvData.xy;
    output.normalView = normalize(mul((float3x3)scene.viewMatrix, mul((float3x3)quadParams.normalMatrix, input.normal)));

    if (quadParams.selected < 0)
    {
        output.position.w += 0.00001 * quadParams.tintColor.x;
    }

    return output;
}
