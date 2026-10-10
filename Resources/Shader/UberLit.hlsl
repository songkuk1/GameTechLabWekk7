#include "UberLit.hlsli"

#pragma pack_matrix(row_major)
#define LIGHTING_MODEL_GOURAUD 1
#define LIGHTING_MODEL_LAMBERT 0
#define LIGHTING_MODEL_PHONG   0
/*
cbuffer PerObject : register(b0)
{
    row_major matrix World; //16 Bytes
    row_major matrix ViewProjection; //16Bytes
    float3 CameraPosition; // 9 Bytes
};

cbuffer Lighting : register(b2)
{
    FAmbientLightInfo Ambient;
    FDirectionalLightInfo Directional;
    FPointLightInfo PointLights[NUM_POINT_LIGHT];
    FSpotLightInfo SpotLights[NUM_SPOT_LIGHT];
};
*/
PS_INPUT mainVS(uint VertexID : SV_VertexID)
{
    PS_INPUT Output = (PS_INPUT) 0;

    float2 UV = float2(
        (VertexID << 1) & 2,
        VertexID & 2);

    Output.position = float4(
        UV * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f),
        0.0f,
        1.0f);

    Output.uv = UV;
    Output.color = float4(1, 1, 1, 1);

    return Output;
}

float4 mainPS(PS_INPUT Input) : SV_TARGET
{
    //float4 finalPixel = TextureColor;
    //finalPixel += Emissive;
#if LIGHTING_MODEL_GOURAUD
    return float4(1,0,0,1);
#elif LIGHTING_MODEL_LAMBERT
    return float4(0,1,0,1);

    /*
    finalPixel += CalculateAmbientLight(...);
    for(It : PointLights)
    {
        finalPixel += CalculatePointLight(...);
    }
    */
#elif LIGHTING_MODEL_PHONG
       return float4(0,0,1,1);
    // Specular Reflectance
#endif
    return float4(1, 1, 1, 1);
}