#define NUM_POINT_LIGHT 4
#define NUM_SPOT_LIGHT 4
struct VS_INPUT
{
    float3 position : POSITION;
    float2 uv : TEXCOORD0;
    float4 color : COLOR;

};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 color : COLOR;
};


struct FAmbientLightInfo
{
};

struct FDirectionalLightInfo
{
};

struct FPointLightInfo
{
};

struct FSpotLightInfo
{
};

float4 CalculateAmbientLight(FAmbientLightInfo info)
{
    float4 result;
    
    return result;
}