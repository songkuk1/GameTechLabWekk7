#pragma pack_matrix(row_major)

struct VSInput
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float4 Color : COLOR;
    float2 UV : TEXCOORD0;
};

struct PSInput
{
    float4 Position : SV_POSITION;
    float3 Normal : NORMAL;
    float4 Color : COLOR;
    float2 UV : TEXCOORD0;
    float3 worldPos : TEXCOORD1;
};

cbuffer ViewConstants : register(b0)
{
    matrix VP;
};
cbuffer WorldConstants : register(b2)
{
    matrix World;
};

struct AmbientLightInfo
{
    float4 ColorIntensity;
};
struct DirectionalLightInfo
{
    float4 ColorIntensity;
    float4 Direction;
};

struct PointLightInfo
{
    float4 ColorIntensity;
    float3 Position;
    float AttenuationRadius;
    float Falloff;
    float3 _pad;
};

struct SpotLightInfo
{
    float4 ColorIntensity;
    float3 Position;
    float AttenuationRadius;
    float3 Direction;
    float Falloff;
    float CosInner;
    float CosOuter;
    float2 _pad; 
};

cbuffer LightBuffer : register(b4)
{
    AmbientLightInfo AmbientLight;
    DirectionalLightInfo DirectionalLight;
    uint PointLightCount;
    uint SpotLightCount;
    float2 _lightPad;
};

StructuredBuffer<PointLightInfo> g_PointLights : register(t1);
StructuredBuffer<SpotLightInfo> g_SpotLights : register(t2);

float DistanceAtten(float Dist, float Radius, float Falloff)
{
    return pow(saturate(1.0 - Dist / Radius), Falloff);
}

float3 CalcPointLight(PointLightInfo L, float3 WorldPos, float3 N)
{
    float3 ToLight = L.Position - WorldPos;
    float Dist = length(ToLight);
    float3 Dir = ToLight / Dist;

    float NdotL = saturate(dot(N, Dir));
    float Atten = DistanceAtten(Dist, L.AttenuationRadius, L.Falloff);
    return L.ColorIntensity.rgb * L.ColorIntensity.w * NdotL * Atten;
}

float3 CalcSpotLight(SpotLightInfo L, float3 WorldPos, float3 N)
{
    float3 ToLight = L.Position - WorldPos;
    float Dist = length(ToLight);
    float3 Dir = ToLight / Dist;

    float CosAngle = dot(-Dir, normalize(L.Direction)); 
    float ConeAtten = smoothstep(L.CosOuter, L.CosInner, CosAngle);

    float NdotL = saturate(dot(N, Dir));
    float Atten = DistanceAtten(Dist, L.AttenuationRadius, L.Falloff) * ConeAtten;
    return L.ColorIntensity.rgb * L.ColorIntensity.w * NdotL * Atten;
}

float3 CalcAllLights(float3 WorldPos, float3 N)
{
    float3 Result = AmbientLight.ColorIntensity.rgb * AmbientLight.ColorIntensity.w;

    float3 DirL = -normalize(DirectionalLight.Direction.xyz);
    Result += DirectionalLight.ColorIntensity.rgb * DirectionalLight.ColorIntensity.w
            * saturate(dot(N, DirL));

    for (uint i = 0; i < PointLightCount; ++i)
        Result += CalcPointLight(g_PointLights[i], WorldPos, N);
    for (uint j = 0; j < SpotLightCount; ++j)
        Result += CalcSpotLight(g_SpotLights[j], WorldPos, N);
    return Result;
}

PSInput mainVS(VSInput Input)
{
    PSInput Out;
    float4 WorldPos = mul(float4(Input.Position, 1.0f), World);
    float3 WorldN = normalize(mul(float4(Input.Normal, 0.0f), World).xyz); // 비균등 스케일이면 역전치 행렬 필요

    Out.Position = mul(WorldPos, VP);
    Out.worldPos = WorldPos.xyz;
    Out.Normal = WorldN;
    Out.UV = Input.UV;

#if LIGHTING_MODEL_GOURAUD
    float3 Light = CalcAllLights(WorldPos.xyz, WorldN);
    Out.Color    = float4(Light * Input.Color.rgb, Input.Color.a);   
#else
    Out.Color = Input.Color; // Lambert/Phong은 PS에서 계산
#endif
    return Out;
}

float4 mainPS(PSInput Input) : SV_Target
{
#if LIGHTING_MODEL_GOURAUD
    return Input.Color;
#else
    float3 N = normalize(Input.Normal);
    return float4(CalcAllLights(Input.worldPos, N) * Input.Color.rgb, Input.Color.a);
#endif
}