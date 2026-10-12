#define NUM_POINT_LIGHT 4
#define NUM_SPOT_LIGHT 4
struct VS_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;

};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
    float3 worldPos : TEXCOORD1;
    float3 diffuse : TEXCOORD2;
    float3 specular : TEXCOORD3;
    
};  


// Light 관련 데이터는 정규화 되었다고 가정한다.
struct FAmbientLightInfo
{
    float3 Color;
    float Intensity;
};

struct FDirectionalLightInfo 
{
    float3 Color;
    float Intensity;
    
    float3 Direction;
    float Padding;
};

struct FPointLightInfo
{
    float3 Position; 
    float AttenuationRadius; 
    //16 Bytes
    
    float3 Color; 
    float Intensity;
    //32 Bytes
    
    float FalloffExponent;
    float3 Padding;
    //48 Bytes
};

struct FSpotLightInfo
{
    float3 Position;
    float AttenuationRadius;
    //16 Bytes
    
    float3 Color;
    float Intensity;
    //32 Bytes
    
    float3 Direction;
    float FalloffExponent;
    //48 Bytes
    
    float InnerCos; //theta/2
    float OuterCos; //theta/2
    float2 Padding;
    
};

struct FLightContribution
{
    float3 Diffuse;
    float3 Specular; // Lambert에서는 0
};
float3 CalculateAmbientLight(FAmbientLightInfo Light)
{
    return Light.Intensity * Light.Color;
    
}
FLightContribution CalculateDirectionalLight(FDirectionalLightInfo Light, float3 N, float3 V,float Shiniess)
{
    FLightContribution output;
    float3 lightAmount;
    //Diffuse
    lightAmount = Light.Intensity * Light.Color;
    output.Diffuse = lightAmount * saturate(dot(Light.Direction, N));
    //Specular
    float3 H = (V + Light.Direction) / max(length(V + Light.Direction), 0.00001f);
    float alpha = max(dot(H, N), 0);
    output.Specular = lightAmount * pow(alpha, Shiniess);
    
    return output;
}

FLightContribution CalculatePointLight(FPointLightInfo Light, float3 WorldPos, float3 N, float3 V, float Shiniess)
{
    FLightContribution output;
    float3 L = Light.Position - WorldPos;
    float distance= length(L);
    L /= distance;
    float3 lightAmount = Light.Intensity * Light.Color * 
                         pow(saturate(1 - distance / Light.AttenuationRadius), Light.FalloffExponent);

    //Diffuse 계산
    
    output.Diffuse = lightAmount * saturate(dot(L, N));
    
    //Specular
    
    float3 H = (L + V) / length(L + V);
    float alpha = max(dot(H, N), 0);
    output.Specular = lightAmount * pow(alpha, Shiniess);
    return output;
    
    
}
FLightContribution CalculateSpotLight(FSpotLightInfo Light, float3 WorldPos, float3 N, float3 V,float Shiniess)
{
    FLightContribution output;
    float3 L = Light.Position - WorldPos;
    float distance = length(L);
    L /= distance;
    float3 lightAmount = Light.Intensity * Light.Color * saturate(dot(L, N)) *
                  pow(saturate(1 - distance / Light.AttenuationRadius), Light.FalloffExponent);

    
    //Diffuse 계산
    float cosW = dot(L, Light.Direction);
    output.Diffuse = lightAmount * saturate((cosW - Light.OuterCos) / max(Light.InnerCos - Light.OuterCos, 0.00001f));
    
    //Specular 계산
    
    float3 H = (L + V) / length(L + V);
    float alpha = max(dot(H, N), 0);
    output.Specular = lightAmount * pow(alpha, Shiniess);

    return output;
    
}