#include "EnginePCH.h"

#include "LightRenderer.h"
#include "RenderCommand.h"
#include "RenderResourceManager.h"

bool FLightRenderer::Init()
{
	DirectionalLightConstantBuffer = RenderCommand::CreateConstantBuffer(sizeof(FDirectionalLightConstants));

	if (!DirectionalLightConstantBuffer)
	{
		return false;
	}

	return true;
}

void FLightRenderer::UpadateConstants(FDirectionalLightConstants InConstant)
{
	Constants = InConstant;
}

void FLightRenderer::OnRender()
{
	// ContantBuffer 업데이트 
	RenderCommand::UpdateBufferData(DirectionalLightConstantBuffer.get(), &Constants);

	// ConstantBuffer 연결  0 : DirectionalLight
	RenderCommand::BindConstantBuffer(0, DirectionalLightConstantBuffer.get(), EShaderBindFlagBits::Pixel); 
}