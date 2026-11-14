module SurfaceModifier;

public struct SurfaceContext
{
    public uint   faceID;
    public uint   vertexID;
    public float3 posWorld;
    public float3 normalView;
    public float3 baseColor;
};

public interface ISurfaceModifier
{
    float3 evaluateAlbedo(SurfaceContext ctx);
};
