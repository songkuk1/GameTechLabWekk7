#pragma once

#include "Render/Buffer.h"
#include "Render/PipelineState.h"
#include "Core/EngineString.h"
#include "Render/Renderer.h"
#include "DirectionalLightInfo.h"

struct alignas(16) FDirectionalLight
{
	FVector4 Direction; // xyz: 방향, w: 패딩
	FVector4 ColorIntensity; // rgb: 색상, w: 강도
	FVector4 EXP; // x: ?, y: ?, z :, w: ? (추후 추가 입력값 대비)
};

struct alignas(16) FLightConstants
{
	FVector4 Direction; // xyz: 방향, w: 패딩
	FVector4 ColorIntensity; // rgb: 색상, w: 강도
	FVector4 EXP; // x: ?, y: ?, z :, w: ? (추후 추가 입력값 대비)
};

struct alignas(16) FDirectionalLightConstants
{
	FDirectionalLight Light[MaxDirectLights];
	uint32 LightCount;
	uint32 Padding[3] = {};
};

class FLightRenderer
{
public:
	FLightRenderer() = default;
	~FLightRenderer() = default;

	FLightRenderer(const FLightRenderer&) = delete;
	FLightRenderer operator=(const FLightRenderer&) = delete;

	bool Init();
	void UpadateConstants(FDirectionalLightConstants InConstant);
	void OnRender();

private:
	FDirectionalLightConstants Constants;
	TUniquePtr<FConstantBuffer> DirectionalLightConstantBuffer;
};