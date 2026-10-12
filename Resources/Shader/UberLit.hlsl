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

cbuffer CameraConstants : register(b5)
{
    float3 CameraPosition;
    float _cameraPad;
};

StructuredBuffer<PointLightInfo> g_PointLights : register(t1);
StructuredBuffer<SpotLightInfo> g_SpotLights : register(t2);

float DistanceAtten(float Dist, float Radius, float Falloff)
{
    return pow(saturate(1.0 - Dist / Radius), Falloff);
}

// 반사 지수. 클수록 하이라이트가 작고 날카롭다. 나중에 머티리얼 파라미터로 뺀다.
static const float Shininess = 64.0f;

float3 CalcSpecular(float3 N, float3 L, float3 V, float3 Radiance)
{
    float3 H = normalize(L + V);
    return Radiance * pow(saturate(dot(N, H)), Shininess);
}

struct FLightResult
{
    float3 Diffuse;
    float3 Specular;
};

FLightResult CalcLightTerm(float3 N, float3 L, float3 V, float3 Radiance)
{
    FLightResult R;
    float NdotL = saturate(dot(N, L));
    R.Diffuse = Radiance * NdotL;
    R.Specular = 0;

#if LIGHTING_MODEL_PHONG
    // 빛이 닿지 않는 뒷면에는 하이라이트를 만들지 않는다
    if (NdotL > 0.0f)
        R.Specular = CalcSpecular(N, L, V, Radiance);
#endif
    return R;
}

FLightResult CalcPointLight(PointLightInfo L, float3 WorldPos, float3 N, float3 V)
{
    float3 ToLight = L.Position - WorldPos;
    float Dist = length(ToLight);
    float3 Dir = ToLight / max(Dist, 0.0001f);

    float Atten = DistanceAtten(Dist, L.AttenuationRadius, L.Falloff);
    return CalcLightTerm(N, Dir, V, L.ColorIntensity.rgb * L.ColorIntensity.w * Atten);
}

FLightResult CalcSpotLight(SpotLightInfo L, float3 WorldPos, float3 N, float3 V)
{
    float3 ToLight = L.Position - WorldPos;
    float Dist = length(ToLight);
    float3 Dir = ToLight / max(Dist, 0.0001f);

    float CosAngle = dot(-Dir, normalize(L.Direction));
    float ConeAtten = smoothstep(L.CosOuter, L.CosInner, CosAngle);
    //ConeAtten = saturate((L.CosOuter - CosAngle) / (L.CosOuter - L.CosInner));;
    float Atten = DistanceAtten(Dist, L.AttenuationRadius, L.Falloff) * ConeAtten;
    return CalcLightTerm(N, Dir, V, L.ColorIntensity.rgb * L.ColorIntensity.w * Atten);
}


FLightResult CalcAllLights(float3 WorldPos, float3 N, float3 V)
{
    //Ambient 값 
    FLightResult Result;
    Result.Diffuse = AmbientLight.ColorIntensity.rgb * AmbientLight.ColorIntensity.w;
    Result.Specular = 0;

    // Direction이 0이면(Directional 라이트 없음) normalize가 NaN이 되어 결과 전체가 깨지므로 길이로 거른다
    float3 DirLightDir = DirectionalLight.Direction.xyz;

    if (dot(DirLightDir, DirLightDir) > 0.0f)
    {
        float3 DirL = -normalize(DirLightDir);
        FLightResult R = CalcLightTerm(N, DirL, V, DirectionalLight.ColorIntensity.rgb * DirectionalLight.ColorIntensity.w);
        Result.Diffuse += R.Diffuse;
        Result.Specular += R.Specular;
    }

    for (uint i = 0; i < PointLightCount; ++i)
    {
        FLightResult R = CalcPointLight(g_PointLights[i], WorldPos, N, V);
        Result.Diffuse += R.Diffuse;
        Result.Specular += R.Specular;
    }

    for (uint j = 0; j < SpotLightCount; ++j)
    {
        FLightResult R = CalcSpotLight(g_SpotLights[j], WorldPos, N, V);
        Result.Diffuse += R.Diffuse;
        Result.Specular += R.Specular;
    }
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
    float3 V = normalize(CameraPosition - WorldPos.xyz);
    FLightResult Light = CalcAllLights(WorldPos.xyz, WorldN, V);
    Out.Color    = float4(Light.Diffuse * Input.Color.rgb + Light.Specular, Input.Color.a);
#else
    Out.Color = Input.Color; // Lambert/Phong은 PS에서 계산
#endif
    return Out;
}

float4 mainPS(PSInput Input) : SV_Target
{
    float4 finalPixel = Input.Color;
    
#if LIGHTING_MODEL_GOURAUD
#elif LIGHTING_MODEL_LAMBERT || LIGHTING_MODEL_PHONG
   
    float3 N = normalize(Input.Normal);
    float3 V = normalize(CameraPosition - Input.worldPos);
    FLightResult Light = CalcAllLights(Input.worldPos, N, V);   // Lambert면 Specular는 0
    finalPixel = float4(Light.Diffuse * Input.Color.rgb + Light.Specular, Input.Color.a);
#endif
    
    return finalPixel;

}