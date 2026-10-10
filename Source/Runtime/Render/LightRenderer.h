#pragma once
#pragma once
#include "Render/Buffer.h"
#include "Render/PipelineState.h"
#include "Core/EngineString.h"
#include "Render/Renderer.h"

struct FLightConstants
{
	FMatrix InverseViewProjection;
	FVector CameraPosition;
	float Padding = 0.0f;
};


class FLightRenderer
{
public:
	FLightRenderer() = default;
	~FLightRenderer() = default;


	FLightRenderer(const FLightRenderer&) = delete;
	FLightRenderer operator=(const FLightRenderer&) = delete;

	bool Init();
	void OnRender(FTexture2D* depthTexture, const FMatrix& ViewProjection, const FVector& CameraPosition);
private:


	FShaderProgram* Shader = nullptr;
	FPipelineState PipelineState;
	TUniquePtr<FConstantBuffer> ConstantBuffer;

};