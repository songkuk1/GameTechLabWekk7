#include "EnginePCH.h"

#include "LightRenderer.h"
#include "RenderCommand.h"
#include "RenderResourceManager.h"

bool FLightRenderer::Init()
{
	LightingModelConstantBuffer = RenderCommand::CreateConstantBuffer(sizeof(FLightingModelConstant));
	DirectionalLightConstantBuffer = RenderCommand::CreateConstantBuffer(sizeof(FDirectionalLightConstants));
	PointLightConstantBuffer = RenderCommand::CreateConstantBuffer(sizeof(FPointLightConstants));
	SpotLightConstantBuffer = RenderCommand::CreateConstantBuffer(sizeof(FSpotLightConstants));
	AmbientLightConstantBuffer = RenderCommand::CreateConstantBuffer(sizeof(FAmbientLightConstants));

	LMConstant.LightingModel = LightingModel::LM_BlinnPhong; // Todo : UI에 변경 트리거 추가

	if (!DirectionalLightConstantBuffer || !PointLightConstantBuffer || !SpotLightConstantBuffer || !AmbientLightConstantBuffer)
	{
		return false;
	}

	// ConstantBuffer 연결  
	// * 3 : LightingModel 4 : DirectionalLight * 5 : PointLight * 6 : SpotLight * 7 : AmbientLight
	RenderCommand::BindConstantBuffer(3, LightingModelConstantBuffer.get(), EShaderBindFlagBits::Pixel | EShaderBindFlagBits::Vertex);
	RenderCommand::BindConstantBuffer(4, DirectionalLightConstantBuffer.get(), EShaderBindFlagBits::Pixel | EShaderBindFlagBits::Vertex);
	RenderCommand::BindConstantBuffer(5, PointLightConstantBuffer.get(), EShaderBindFlagBits::Pixel | EShaderBindFlagBits::Vertex);
	RenderCommand::BindConstantBuffer(6, SpotLightConstantBuffer.get(), EShaderBindFlagBits::Pixel | EShaderBindFlagBits::Vertex);
	RenderCommand::BindConstantBuffer(7, AmbientLightConstantBuffer.get(), EShaderBindFlagBits::Pixel | EShaderBindFlagBits::Vertex);

	return true;
}

void FLightRenderer::UpadateConstants(const FLightingModelConstant& InConstant)
{
	LMConstant = InConstant;
}

void FLightRenderer::UpadateConstants(const FDirectionalLightConstants& InConstant)
{
	DLConstants = InConstant;
}

void FLightRenderer::UpadateConstants(const FPointLightConstants& InConstant)
{
	PLConstants = InConstant;
}

void FLightRenderer::UpadateConstants(const FSpotLightConstants& InConstant)
{
	SLConstants = InConstant;
}

void FLightRenderer::UpadateConstants(const FAmbientLightConstants& InConstant)
{
	ALConstants = InConstant;
}

void FLightRenderer::OnRender()
{
	// ContantBuffer 업데이트 
	RenderCommand::UpdateBufferData(LightingModelConstantBuffer.get(), &LMConstant);
	RenderCommand::UpdateBufferData(DirectionalLightConstantBuffer.get(), &DLConstants);
	RenderCommand::UpdateBufferData(PointLightConstantBuffer.get(), &PLConstants);
	RenderCommand::UpdateBufferData(SpotLightConstantBuffer.get(), &SLConstants);
	RenderCommand::UpdateBufferData(AmbientLightConstantBuffer.get(), &ALConstants);
}