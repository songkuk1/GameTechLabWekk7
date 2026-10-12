#pragma pack_matrix(row_major)
cbuffer Viewconstants : register(b0)
{
    matrix VP;
    float4 CameraPos;
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

struct PointLight
{
    float4 PositionRadius;
    float4 ColorIntensity;
    float4 FalloffEnabled;
};

struct DirectionalLight
{
    float4 Direction;
    float4 ColorIntensity;
    float4 EXP; // NONE
};

struct SpotLight
{
    float4 Direction; 
    float4 PositionRadius; 
    float4 ColorIntensity; 
    float4 FalloffEnabled;
    float4 InnerOuter; 
};

cbuffer LightingModelComponents : register(b3)
{
    uint LightingModel;
    uint3 LMPadding;
}

cbuffer DirectionalLightComponents : register(b4)
{
    DirectionalLight DirectionalLights[2];
    uint DirectionalLightCount;
    uint3 DLCPadding;
}

cbuffer PointLightConstants : register(b5)
{
    PointLight PointLights[16];
    uint PointLightCount;
    float3 PointLightPadding;
};

cbuffer SpotLightConstants : register(b6)
{
    SpotLight SpotLights[16];
    uint SpotLightCount;
    float3 SpotLightPadding;
};

cbuffer AmbientLightConstants : register(b7)
{
    float4 ALColorIntensity;
};

struct VS_INPUT
{
    float3 p : POSITION; // Input position from vertex buffer
    float3 n : NORMAL;
    float4 c : COLOR; // Input color from vertex buffer
    float2 t : TEXCOORD;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
    float3 worldPos : TEXCOORD1;
};

Texture2D g_txColor : register(t0);
SamplerState g_Sample : register(s0);

float3 CalculateDirectional(PS_INPUT input, uint LightingModel, out float3 totalSpecular)
{
    float3 N = normalize(input.normal);
    float3 totalDirectionalLight = float3(0, 0, 0); // DirectionLight
    totalSpecular = float3(0, 0, 0);
    for (uint dli = 0; dli < DirectionalLightCount; dli++)
    {
        DirectionalLight Light = DirectionalLights[dli];
        float3 L = normalize(-Light.Direction.xyz);
        float directionNdotL = saturate(dot(N, L));
        float3 directionalLight = Light.ColorIntensity.xyz * Light.ColorIntensity.w * directionNdotL;
        if (LightingModel == 2 && directionNdotL > 0.0f)
        {
            float Shininess = 32.0f;
            float3 V = normalize(CameraPos.xyz - input.worldPos);
            float3 H = normalize(L + V);
            float specular = pow(max(dot(H, N), 0),Shininess);
            totalSpecular += Light.ColorIntensity.rgb * Light.ColorIntensity.w * specular;
        }
        totalDirectionalLight += directionalLight;
    }
    return totalDirectionalLight;
}

float3 CalculatePoint(PS_INPUT input, uint LightingModel, out float3 totalSpecular)
{
    float3 N = normalize(input.normal);
    float3 totalPointLight = float3(0, 0, 0); // PointLight
    totalSpecular = float3(0, 0, 0);
    for (uint i = 0; i < PointLightCount; ++i)
    {
        PointLight pointlight = PointLights[i];
        float4 PositionRadius = pointlight.PositionRadius;
        float4 ColorIntensity = pointlight.ColorIntensity;
        float4 FalloffEnabled = pointlight.FalloffEnabled;
        
        float distance = length(PositionRadius.xyz - input.worldPos);
        float radius = max(PositionRadius.w, 0.001f);
        
        float Basefalloff = saturate(1.0f - distance / radius);
        float falloff = pow(Basefalloff, max(FalloffEnabled.x, 0.001f));
        
        float enabled = FalloffEnabled.y;
        float intensity = ColorIntensity.w;
        
        float3 lightDir = normalize(PositionRadius.xyz - input.worldPos);
        
        float pointNdotL = saturate(dot(N, lightDir));
        float3 pointLight = ColorIntensity.rgb * pointNdotL * falloff * enabled * intensity;
        
        if (LightingModel == 2 && pointNdotL > 0.0f)
        {
            float Shininess = 32.0f;
            float3 L = lightDir;
            float3 V = normalize(CameraPos.xyz - input.worldPos);
            float3 H = normalize(L + V);
            float specular = pow(max(dot(H, N), 0), Shininess);
            totalSpecular += ColorIntensity.rgb * falloff * enabled * intensity * specular;
        }
        
        totalPointLight += pointLight;
    }
    return totalPointLight;
}

float3 CalculateSpot(PS_INPUT input, uint LightingModel, out float3 totalSpecular)
{
    float3 N = normalize(input.normal);
    float3 totalSpotLight = float3(0, 0, 0); // SpotLight
    totalSpecular = float3(0, 0, 0);
    for (uint spi = 0; spi < SpotLightCount; spi++)
    {
        SpotLight slight = SpotLights[spi];
        
        float4 D = slight.Direction;
        float4 CI = slight.ColorIntensity;
        float4 FOE = slight.FalloffEnabled;
        float4 IO = slight.InnerOuter;
        float4 PR = slight.PositionRadius;
     
        float3 direction = normalize(PR.xyz - input.worldPos);
        float distance = length(PR.xyz - input.worldPos);
        float radius = max(PR.w, 0.001f);
        float Basefalloff = saturate(1.0f - distance / radius);
        
        float cosInner = cos(radians(IO.x));
        float cosOuter = cos(radians(IO.y));
        
        float Intensity = CI.a; // 강도
        float LB = saturate(dot(N, direction)); // 표면에서 광원으로 향하는 방향으로 램버트 계산
        float FO = pow(Basefalloff, max(FOE.x, 0.0001f)); // 거리별 감쇠
        float FE = FOE.y; // 거리감쇠 사용 여부
        
        float CF = 1.0f; // 콘 감쇠
        float Pcos = dot(D.xyz, -direction);
        if (cosInner >= Pcos && Pcos >= cosOuter)
        {
            float a = saturate((Pcos - cosOuter) / max(cosInner - cosOuter, 0.0001f));
            CF *= a;
        }
        if (cosOuter > Pcos)
        {
            CF = 0.0f;
        }
        
        float3 sLight = CI.rgb * LB * Intensity * FO * CF * FE;
        
        if (LightingModel == 2 && LB > 0.0f)
        {
            float Shininess = 32.0f;
            float3 L = direction;
            float3 V = normalize(CameraPos.xyz - input.worldPos);
            float3 H = normalize(L + V);
            float specular = pow(max(dot(H, N), 0), Shininess);
            totalSpecular += CI.rgb * Intensity * FO * CF * FE * specular;
        }
        totalSpotLight += sLight;
    }
    return totalSpotLight;
}

float3 CalculateAmbient()
{
    float3 AmbientColor = ALColorIntensity.rgb * ALColorIntensity.a;
    return AmbientColor;
}

PS_INPUT mainVS(VS_INPUT input)
{
    PS_INPUT output;

    output.worldPos = mul(float4(input.p, 1.0f), World).xyz;
    output.position = mul(mul(float4(input.p, 1.0f), World), VP);
    
    // w=0으로 이동 성분을 빼고 월드 공간으로 보낸다. 비균등 스케일이면 역전치가 필요하다.
    output.normal = mul(float4(input.n, 0.0f), World).xyz;
    output.color = input.c;
    output.uv = input.t;
    
    if(LightingModel == 0) // 구로 셰이딩
    {
        float3 directionalSpecular;
        float3 pointSpecular;
        float3 spotSpecular;
        float3 Color = CalculateAmbient()
            + CalculateDirectional(output, LightingModel, directionalSpecular)
            + CalculatePoint(output, LightingModel, pointSpecular)
            + CalculateSpot(output, LightingModel, spotSpecular);
        output.color = float4(Color, 1.0f);
    }
   
    return output;
}

float4 mainPS(PS_INPUT input) : SV_TARGET
{
    float4 texColor = g_txColor.Sample(g_Sample, input.uv + UVOffset);
    float4 albedo = texColor * BaseColor;
    
    if (LightingModel == 0) // 구로 셰이딩
    {
        return float4(albedo.rgb * input.color.rgb, bOpaque > 0.5f ? 1.0f : albedo.a);;
    }
 
    float3 AmbientColor = CalculateAmbient();

    float3 directionalSpecular;
    float3 pointSpecular;
    float3 spotSpecular;

    float3 totalDirectionalLight = CalculateDirectional(input, LightingModel, directionalSpecular);
    
    float3 totalPointLight = CalculatePoint(input, LightingModel, pointSpecular);
    
    float3 totalSpotLight = CalculateSpot(input, LightingModel, spotSpecular);
    
    // Opaque는 알파를 1로 고정한다. 뷰포트 RT를 ImGui가 알파 블렌딩으로 그리므로 알파가 남으면 비쳐 보인다
    float alpha = bOpaque > 0.5f ? 1.0f : albedo.a;
   
    // 텍스처 색은 확산광에 적용하고, 스페큘러는 광원 색으로 별도 합산한다.
    float3 finalRGB = albedo.rgb * (AmbientColor + totalDirectionalLight + totalPointLight + totalSpotLight)
        + directionalSpecular + pointSpecular + spotSpecular;
    return float4(finalRGB, alpha);
}
