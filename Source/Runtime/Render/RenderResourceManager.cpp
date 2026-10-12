#include "EnginePCH.h"
#include "RenderResourceManager.h"

#include "RenderCommand.h"

#include "RenderUtil.h"



void FRenderResourceManager::ScanShaders(const fs::path& ShaderRoot)
{
	std::error_code ErrorCode;

	if (!fs::exists(ShaderRoot, ErrorCode))
	{
		HTR_LOG(Error, "[Shader] Scan Root Not Found : {}", ShaderRoot.generic_string());
		return;
	}

	for (const fs::directory_entry& Entry : fs::recursive_directory_iterator(ShaderRoot))
	{
		if (!Entry.is_regular_file()) continue;
		if (Entry.path().extension() != ".hlsl") continue;

		FString Path = Entry.path().generic_string();
		FShaderProgram* Shader = LoadOrCompileShader(Path);
		if (Path == "Resources/Shader/UberLit.hlsl")
			DefaultUberLitShader = Shader;
	}
}

void FRenderResourceManager::Shutdown()
{
	Get().DefaultUberLitShader = nullptr;
	Get().UnlitShader = nullptr;
	for (FShaderProgram*& Shader : Get().UberLitShaders)
		Shader = nullptr;
	Get().VertexShaderMap.Empty();
	Get().PixelShaderMap.Empty();
	Get().ShaderProgramMap.Empty();
}

FShaderProgram* FRenderResourceManager::GetShaderProgram(const FString& InPath)
{
	if (TUniquePtr<FShaderProgram>* Found = Get().ShaderProgramMap.Find(InPath))
		return Found->get();

	HTR_LOG(Error, "[Shader] not found: {}", InPath);

	if (TUniquePtr<FShaderProgram>* Fallback =
		Get().ShaderProgramMap.Find("Resources/Shader/DefaultShader.hlsl"))
		return Fallback->get();

	return nullptr;
}

FShaderProgram* FRenderResourceManager::GetUberLitShader(ELightingModel Model)
{
	if (Model < ELightingModel::Gouraud || Model > ELightingModel::BlinnPhong)
		Model = ELightingModel::BlinnPhong;
	return Get().UberLitShaders[static_cast<int32>(Model)];
}

bool FRenderResourceManager::IsUberLitShader(const FShaderProgram* Shader)
{
	if (!Shader) return false;
	if (Shader == Get().DefaultUberLitShader || Shader == Get().UnlitShader) return true;
	for (const FShaderProgram* Variant : Get().UberLitShaders)
		if (Shader == Variant) return true;
	return false;
}

FShaderProgram* FRenderResourceManager::LoadOrCompileShader(const FString& Path, const D3D_SHADER_MACRO* Defines, const FString& Variant)
{ 
	const FString Key = Variant.empty() ? Path : Path + "#" + Variant;
	FString VSCSOPath;
	FString PSCSOPath;
	FShaderByteCode VSCode = RenderUtil::GetOrCompile(Path, "mainVS", EShaderType::Vertex, VSCSOPath,Defines,Variant);
	FShaderByteCode PSCode = RenderUtil::GetOrCompile(Path, "mainPS", EShaderType::Pixel, PSCSOPath,Defines,Variant);

	if (!VSCode.IsValid() || !PSCode.IsValid())
	{
		HTR_LOG(Error, "[Shader] compile failed: {}", Key);
		return nullptr;
	}

	TUniquePtr<FVertexShader> Vs = RenderCommand::CreateVertexShader(VSCode);
	TUniquePtr<FPixelShader>  Ps = RenderCommand::CreatePixelShader(PSCode);

	if (!Vs || !Vs->IsValid() || !Ps || !Ps->IsValid())
	{
		HTR_LOG(Error, "[Shader] device create failed: {}", Key);
		return nullptr;
	}

	FVertexShader* VsRaw = Vs.get();
	FPixelShader* PsRaw = Ps.get();

	VertexShaderMap[VSCSOPath] = std::move(Vs);
	PixelShaderMap[PSCSOPath] = std::move(Ps);
	ShaderProgramMap[Key] = MakeUnique<FShaderProgram>(VsRaw, PsRaw);
	

	HTR_LOG(Info, "[Shader] loaded: {}", Key);
	return ShaderProgramMap[Key].get();
}
void FRenderResourceManager::LoadUberLitVariants() 
{
	const FString Path = "Resources/Shader/UberLit.hlsl";
	
	const D3D_SHADER_MACRO GouraudDefines[] = {
		{ "LIGHTING_MODEL_GOURAUD", "1" }, { "LIGHTING_MODEL_LAMBERT", "0" }, { "LIGHTING_MODEL_PHONG", "0" }, { nullptr, nullptr } };
	const D3D_SHADER_MACRO LambertDefines[] = {
		{ "LIGHTING_MODEL_GOURAUD", "0" }, { "LIGHTING_MODEL_LAMBERT", "1" }, { "LIGHTING_MODEL_PHONG", "0" }, { nullptr, nullptr } };
	const D3D_SHADER_MACRO BlinnPhongDefines[] = {
		{ "LIGHTING_MODEL_GOURAUD", "0" }, { "LIGHTING_MODEL_LAMBERT", "0" }, { "LIGHTING_MODEL_PHONG", "1" }, { nullptr, nullptr } };
	const D3D_SHADER_MACRO UnlitDefines[] = {
		{ "LIGHTING_MODEL_GOURAUD", "0" }, { "LIGHTING_MODEL_LAMBERT", "0" }, { "LIGHTING_MODEL_PHONG", "0" }, { nullptr, nullptr } };

	UberLitShaders[static_cast<int32>(ELightingModel::Gouraud)] = LoadOrCompileShader(Path, GouraudDefines, "GOURAUD");
	UberLitShaders[static_cast<int32>(ELightingModel::Lambert)] = LoadOrCompileShader(Path, LambertDefines, "LAMBERT");
	UberLitShaders[static_cast<int32>(ELightingModel::BlinnPhong)] = LoadOrCompileShader(Path, BlinnPhongDefines, "PHONG");
	UnlitShader = LoadOrCompileShader(Path, UnlitDefines, "UNLIT");


}
