#pragma once

#include "Render/Buffer.h"
#include "Render/PipelineState.h"
#include "Core/EngineString.h"
#include "Render/Renderer.h"
#include "DirectionalLightInfo.h"

constexpr uint32 MaxPointLights = 16;
constexpr uint32 MaxSpotLights = 16;
constexpr uint32 MaxDirectionalLights = 2;

enum class LightingModel : uint32
{
	LM_GOURAUD,
	LM_LAMBERT,
	LM_BlinnPhong
};

struct alignas(16) FLightingModelConstant
{
	LightingModel LightingModel;
	uint32 Padding[3] = {};
};

struct alignas(16) FDirectionalLight
{
	FVector4 Direction; // xyz: 방향, w: 패딩
	FVector4 ColorIntensity; // rgb: 색상, w: 강도
	FVector4 EXP; // x: ?, y: ?, z :, w: ? (추후 추가 입력값 대비)
};

struct alignas(16) FDirectionalLightConstants
{
	FDirectionalLight Light[MaxDirectionalLights];
	uint32 LightCount;
	uint32 Padding[3] = {};
};

struct alignas(16) FPointLight
{
	FVector4 PositionRadius; // xyz: 위치, w: 반경
	FVector4 ColorIntensity; // rgb: 색상, w: 강도
	FVector4 FalloffEnabled; // x: 반경 감쇠, y: 사용 여부, zw: 패딩
};

struct alignas(16) FPointLightConstants
{
	FPointLight Light[MaxPointLights];
	uint32 LightCount;
	uint32 Padding[3] = {};
};

struct alignas(16) FSpotLight
{
	FVector4 Direction; // xyz: 방향, w: 패딩
	FVector4 PositionRadius; // xyz: 위치, w: 반경
	FVector4 ColorIntensity; // rgb: 색상, w: 강도
	FVector4 FalloffEnabled; // x: 반경 감쇠, y: 사용 여부, zw: 패딩
	FVector4 InnerOuter; // x: InnerCone 반각, y: OuterCone 반각, zw: 패딩
};

struct alignas(16) FSpotLightConstants
{
	FSpotLight Light[MaxSpotLights];
	uint32 LightCount;
	uint32 Padding[3] = {};
};

struct alignas(16) FAmbientLightConstants
{
	FVector4 ColorIntensity; // rgb : 색상, w : 강도 
};

class FLightRenderer
{
public:
	FLightRenderer() = default;
	~FLightRenderer() = default;

	FLightRenderer(const FLightRenderer&) = delete;
	FLightRenderer operator=(const FLightRenderer&) = delete;

	bool Init();
	void UpadateConstants(const FLightingModelConstant& InConstant);
	void UpadateConstants(const FDirectionalLightConstants& InConstant);
	void UpadateConstants(const FPointLightConstants& InConstant);
	void UpadateConstants(const FSpotLightConstants& InConstant);
	void UpadateConstants(const FAmbientLightConstants& InConstant);
	void OnRender();

private:
	FLightingModelConstant LMConstant;
	FDirectionalLightConstants DLConstants;
	FPointLightConstants PLConstants;
	FSpotLightConstants SLConstants;
	FAmbientLightConstants ALConstants;

	TUniquePtr<FConstantBuffer> LightingModelConstantBuffer;
	TUniquePtr<FConstantBuffer> DirectionalLightConstantBuffer;
	TUniquePtr<FConstantBuffer> PointLightConstantBuffer;
	TUniquePtr<FConstantBuffer> SpotLightConstantBuffer;
	TUniquePtr<FConstantBuffer> AmbientLightConstantBuffer;
};