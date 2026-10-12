#pragma pack_matrix(row_major)

struct FPointLightInfo
{
    float3 Position;
    float Radius;
    float4 LightColor;
    float Intensity;
    float AttenuationConstant;
    float2 Padding;
};

cbuffer Viewconstants : register(b0)
{
    matrix VP;
};

cbuffer MaterialParams : register(b1)
{
    float4 BaseColor;
    float2 UVOffset;
    float bOpaque;
    float Padding;
};

cbuffer Worldconstants : register(b2)
{
    matrix World;
};

cbuffer Lighting : register(b4)
{
    FPointLightInfo PointLights[16];
    uint PointLightCount;
    float3 LightingPadding;
};

Texture2D g_txColor : register(t0);
SamplerState g_Sample : register(s0);

struct VS_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
    float3 worldPos : TEXCOORD1;
};

struct PS_OUTPUT
{
    float4 color : COLOR;
};


float3 CalculatePointLight(PS_INPUT input, FPointLightInfo PointLightInfo);

PS_INPUT mainVS(VS_INPUT input) 
{
    PS_INPUT output;

    float4 inputPosition = float4(input.position, 1.0f);
    output.worldPos = mul(inputPosition, World).xyz;
    output.position = mul(mul(inputPosition, World), VP);
    
    output.normal = mul(float4(input.normal, 0.0f), World).xyz;
    output.color = input.color;
    output.uv = input.uv;
    return output;
}


float4 mainPS(PS_INPUT input) : SV_TARGET
{
    float4 output = float4(0.0f, 0.0f, 0.0f, 1.0f);
    
    // Mesh 표면색
    float4 texColor = g_txColor.Sample(g_Sample, input.uv + UVOffset);
    float4 albedo = texColor * BaseColor;
    
    float3 totalPointLight = float3(0, 0, 0);
    for (uint i = 0; i < PointLightCount; ++i)
    {
        totalPointLight += CalculatePointLight(input, PointLights[i]);
    }
    
    static const float3 AmbientColor = float3(0.5f, 0.5f, 0.5f);
    
    float alpha = bOpaque > 0.5f ? 1.0f : albedo.a;
    
    output = float4(albedo.rgb * (AmbientColor + totalPointLight), 1.0f);
    
    return output;
}

float3 CalculatePointLight(PS_INPUT input, FPointLightInfo PointLightInfo)
{
    float3 outputColor = float3(0.0f, 0.0f, 0.0f);
    
    float3 lightDir = normalize(PointLightInfo.Position - input.worldPos);
    float pointNdotL = saturate(dot(normalize(input.normal), lightDir));
    float distance = length(input.worldPos.xyz - PointLightInfo.Position);
    float Attenuation = clamp(1.0f - distance / PointLightInfo.Radius, 0.0f, 1.0f);
    Attenuation = pow(Attenuation, PointLightInfo.AttenuationConstant);
    
    outputColor = PointLightInfo.LightColor.rgb * PointLightInfo.Intensity * pointNdotL * Attenuation;
    
    return outputColor;
}
