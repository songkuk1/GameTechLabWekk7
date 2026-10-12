#include "UberLit.hlsli"

#ifndef LIGHTING_MODEL_GOURAUD
#define LIGHTING_MODEL_GOURAUD 0
#endif
#ifndef LIGHTING_MODEL_LAMBERT
#define LIGHTING_MODEL_LAMBERT 0
#endif
#ifndef LIGHTING_MODEL_PHONG
#define LIGHTING_MODEL_PHONG 0
#endif

#define HAS_NORMAL_MAP 1
#pragma pack_matrix(row_major)
cbuffer PerFrame : register(b0)
{
    matrix ViewProjection; // 64 Bytes
    
    float3 CameraPosition; // 12 Bytes
    float b0Padding; // 4 Bytes
};

// 광원의 종류
cbuffer MaterialParams : register(b1)
{
    float4 BaseColor; //16 Bytes;
    
    float2 UVOffset;
    float bOpaque;
    float Shiniess; // 32 Bytes
    
    float3 SpecularColor; 
    
    //float4 EmissiveColor;
}
cbuffer Worldconstants : register(b2)
{
    matrix World;
};

cbuffer Lighting : register(b4)
{
    FAmbientLightInfo Ambient;
    FDirectionalLightInfo Directional;
    FPointLightInfo PointLights[NUM_POINT_LIGHT];
    FSpotLightInfo SpotLights[NUM_SPOT_LIGHT];
    int NumPointLights;
    int NumSpotLights;
    float2 LightingPadding;
};

Texture2D InputTexture : register(t0);
SamplerState LinearClamp : register(s0);

#if HAS_NORMAL_MAP
Texture2D NormalMap  : register(t1);
#endif

PS_INPUT mainVS(VS_INPUT Input)
{
    PS_INPUT output;
    
    float3 worldPos = mul(float4(Input.position, 1.0f), World).xyz;
    output.worldPos = worldPos;
    output.position = mul(mul(float4(Input.position, 1.0f), World), ViewProjection);
    output.normal = mul(float4(Input.normal, 0.0f), World).xyz;
    output.uv = Input.uv;
    output.diffuse = 0;
    output.specular = 0;
#if LIGHTING_MODEL_GOURAUD
    
    float3 N = normalize(output.normal); // 현재 표면이 향하고 있는 방향
    float3 V = normalize(CameraPosition - worldPos);

    output.diffuse = CalculateAmbientLight(Ambient);
    FLightContribution Calout = CalculateDirectionalLight(Directional, N, V, Shiniess);
    output.diffuse += Calout.Diffuse;
    output.specular += Calout.Specular;
   
    //PointLight    
    for (int i = 0; i < min(NumPointLights, NUM_POINT_LIGHT); ++i)
    {
        Calout = CalculatePointLight(PointLights[i], worldPos, N, V, Shiniess);
        output.diffuse += Calout.Diffuse;
        output.specular += Calout.Specular;
    }
    
    //SpotLight
    for (int j = 0; j < min(NumSpotLights, NUM_SPOT_LIGHT); ++j)
    {
        Calout = CalculateSpotLight(SpotLights[j], worldPos, N, V, Shiniess);
        output.diffuse += Calout.Diffuse;
        output.specular += Calout.Specular;
    }

    
#endif
    // w=0으로 이동 성분을 빼고 월드 공간으로 보낸다. 비균등 스케일이면 역전치가 필요하다.
    output.color = Input.color;
    return output;
}

float4 mainPS(PS_INPUT Input) : SV_TARGET
{
    float3 N = normalize(Input.normal); // 현재 표면이 향하고 있는 방향
    float3 V = normalize(CameraPosition - Input.worldPos);
    float4 texColor = InputTexture.Sample(LinearClamp, Input.uv + UVOffset);
    float4 albedo = texColor * BaseColor;
    float alpha = bOpaque > 0.5f ? 1.0f : albedo.a;
    float3 finalRGB = 0;
    float3 finalDiffuse = 0;
    float3 finalSpecular = 0;
    
#if LIGHTING_MODEL_GOURAUD
    finalDiffuse = Input.diffuse;
    finalSpecular = Input.specular;

#elif LIGHTING_MODEL_LAMBERT
    //Lambert Shading
    
    //Ambient, Directional
    finalDiffuse = CalculateAmbientLight(Ambient) + CalculateDirectionalLight(Directional, N, V, Shiniess).Diffuse;
    
    //PointLight    
    for (int i = 0; i < min(NumPointLights, NUM_POINT_LIGHT); ++i)
    {
        finalDiffuse += CalculatePointLight(PointLights[i], Input.worldPos, N, V, Shiniess).Diffuse;
    }
    
    //SpotLight
    for (int j = 0; j < min(NumSpotLights, NUM_SPOT_LIGHT); ++j)
    {
        finalDiffuse += CalculateSpotLight(SpotLights[j], Input.worldPos, N, V, Shiniess).Diffuse;
    }
    
#elif LIGHTING_MODEL_PHONG
    //Blinn-Phong Shading
    
    finalDiffuse += CalculateAmbientLight(Ambient);
    FLightContribution Calout = CalculateDirectionalLight(Directional, N, V, Shiniess);
    finalDiffuse += Calout.Diffuse;
    finalSpecular += Calout.Specular;
   
    //PointLight    
    for (int i = 0; i < min(NumPointLights, NUM_POINT_LIGHT); ++i)
    {
        Calout = CalculatePointLight(PointLights[i], Input.worldPos, N, V, Shiniess);
        finalDiffuse += Calout.Diffuse;
        finalSpecular += Calout.Specular;
    }
    
    //SpotLight
    for (int j = 0; j < min(NumSpotLights, NUM_SPOT_LIGHT); ++j)
    {
        Calout = CalculateSpotLight(SpotLights[j], Input.worldPos, N, V, Shiniess);
        finalDiffuse += Calout.Diffuse;
        finalSpecular += Calout.Specular;
    }
    
#else 
   // unlit
    return float4(albedo.rgb,alpha);
#endif

    finalRGB = albedo.rgb * finalDiffuse + finalSpecular;
    return float4(finalRGB, alpha);
}
