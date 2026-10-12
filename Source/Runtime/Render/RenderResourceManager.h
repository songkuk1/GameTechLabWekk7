#pragma once

#include "Shader.h"
#include "RenderEnums.h"

class FRenderResourceManager
{
public:
	static FRenderResourceManager& Get()
	{
		static FRenderResourceManager* Instance = new FRenderResourceManager();
		return *Instance;
	}

	static void Init()
	{
		Get().ScanShaders("Resources/Shader");
		Get().LoadUberLitVariants();
	}

	void ScanShaders(const fs::path& ShaderRoot);

	static void Shutdown();

	static FShaderProgram* GetShaderProgram(const FString& InPath);
	static FShaderProgram* GetUberLitShader(ELightingModel Model);
	static FShaderProgram* GetUnlitShader() { return Get().UnlitShader; }
	static bool IsUberLitShader(const FShaderProgram* Shader);
private:
	FShaderProgram* LoadOrCompileShader(const FString& Path, const D3D_SHADER_MACRO* Defines=nullptr , const FString& Variant = "");
	void LoadUberLitVariants();
	FShaderProgram* DefaultUberLitShader = nullptr;
	FShaderProgram* UberLitShaders[3]{};
	FShaderProgram* UnlitShader = nullptr;
	TMap<FString, TUniquePtr<FVertexShader>> VertexShaderMap;
	TMap<FString, TUniquePtr<FPixelShader>>  PixelShaderMap;
	TMap<FString, TUniquePtr<FShaderProgram>> ShaderProgramMap;
};
