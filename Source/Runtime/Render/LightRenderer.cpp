#include "EnginePCH.h"
#include "LightRenderer.h"
#include "RenderCommand.h"
#include "RenderResourceManager.h"

bool FLightRenderer::Init()
{

	Shader = FRenderResourceManager::GetShaderProgram("Resources/Shader/UberLit.hlsl");
	if (!Shader)
	{
		HTR_LOG(Error, "[UberLit] shader not found");
		return false;
	}


	ConstantBuffer = RenderCommand::CreateConstantBuffer(sizeof(FLightConstants));

	PipelineState.Shader = Shader;
	PipelineState.Topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	// 셰이더가 알파에 안개 양을 담아 보내므로, 장면 위에 섞어서 덮는다
	PipelineState.BlendState = EBlendState::Opaque;
	PipelineState.DepthStencilState = EDepthStencilState::Disabled;
	PipelineState.RasterizerState = ERasterizerState::SolidNone;

	return true;
}

void FLightRenderer::OnRender(FTexture2D* depthTexture, const FMatrix& ViewProjection, const FVector& CameraPosition)
{

	//fogshader 바인드
	RenderCommand::BindPipelineState(PipelineState);
	RenderCommand::BindShaderResource(0, depthTexture, EShaderBindFlagBits::Pixel);

	FLightConstants Constants;
	Constants.InverseViewProjection = ViewProjection.Inverse();
	Constants.CameraPosition = CameraPosition;
	RenderCommand::UpdateBufferData(ConstantBuffer.get(), &Constants);
	RenderCommand::BindConstantBuffer(0, ConstantBuffer.get(), EShaderBindFlagBits::Pixel);
	RenderCommand::Draw(3);
}
