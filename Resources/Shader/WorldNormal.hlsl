#pragma pack_matrix(row_major)
// 스태틱 메시 기본 셰이더 (UnLit). 라이트 없이 텍스처 * BaseColor만 출력한다.
// Lit 뷰 모드에서는 FRenderer::BindMaterial이 UberLit으로 갈아 끼우므로 입력·슬롯을 그쪽과 같게 둔다.
cbuffer Viewconstants : register(b0)
{
    matrix VP;
};

cbuffer Worldconstants : register(b2)
{
    matrix World;
};

cbuffer MaterialParams : register(b1)
{
    float4 BaseColor;
    float2 UVOffset;
    float bOpaque;
    float Padding;
};

struct VS_INPUT
{
    float3 p : POSITION;
    float3 n : NORMAL;
    float4 c : COLOR;
    float2 t : TEXCOORD;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
};

Texture2D g_txColor : register(t0);
SamplerState g_Sample : register(s0);

PS_INPUT mainVS(VS_INPUT input)
{
    PS_INPUT output;
    output.position = mul(mul(float4(input.p, 1.0f), World), VP);
    output.normal = mul(float4(input.n, 0.0f), World).xyz;
    output.uv = input.t;
    return output;
}

float4 mainPS(PS_INPUT input) : SV_TARGET
{
    float3 color = normalize(input.normal) * 0.5f + 0.5f;
    return float4(color, 1.0);
}
